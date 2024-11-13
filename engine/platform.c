// sokol
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_log.h"

#define PTI_IMPL
#include "pti.h"

// forward declarations
static void init(void);
static void frame(void);
static void cleanup(void);
static void input(const sapp_event *);

int main(int argc, char *argv[]) {
	pti_desc desc = pti_main(argc, argv);

	pti_init(&desc);

	int width = desc.window.width;
	int height = desc.window.height;
	_pti__scale_size_by_flags(&width, &height, desc.window.flags);

	const char *name = desc.window.name ? desc.window.name : "pti";
	sapp_run(&(sapp_desc) {
			.init_cb = init,
			.frame_cb = frame,
			.cleanup_cb = cleanup,
			.event_cb = input,
			.width = width,
			.height = height,
			.window_title = name,
			.logger.func = slog_func,
	});
	return 0;
}

enum {
	TICK_DURATION_NS = 33333333,
	TICK_TOLERANCE_NS = 1000000,
};

static struct {
	struct {
		unsigned int tick;              // the central game tick, this drives the whole game
		unsigned long int laptime_store;// helper variable to measure frame duration
		int tick_accum;                 // helper variable to decouple ticks from frame rate
	} timing;

	sg_pass_action pass_action;
	struct {
		sg_image render_target;
	} offscreen;

	struct {
		sg_pass_action pass_action;
		sg_pipeline pip;
		sg_bindings bind;
	} display;

} state;


static void sokol_init_gfx(void) {
	/* setup sokol-gfx */
	sg_setup(&(sg_desc) {
			.environment = sglue_environment(),
			.buffer_pool_size = 2,
			.image_pool_size = 3,
			.shader_pool_size = 2,
			.pipeline_pool_size = 2,
			.logger.func = slog_func,
	});

	/* a vertex buffer */
	const float vertices[] = {
			-1.0f, 1.0f,  /* top-left */
			1.0f, 1.0f,   /* top-right */
			1.0f, -1.0f,  /* bottom-right */
			-1.0f, -1.0f, /* bottom-left */
	};

	/* an index buffer with 2 triangles */
	const uint16_t indices[] = {0, 1, 2, 0, 2, 3};

	sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc) {
			.data = SG_RANGE(vertices),
			.label = "quad-vertices",
	});

	sg_buffer ibuf = sg_make_buffer(&(sg_buffer_desc) {
			.type = SG_BUFFERTYPE_INDEXBUFFER,
			.data = SG_RANGE(indices),
			.label = "quad-indices",
	});

	const char *display_vs_src = 0;
	const char *display_fs_src = 0;
	switch (sg_query_backend()) {
		case SG_BACKEND_GLCORE:
			display_vs_src =
					"#version 330\n"
					"layout(location=0) in vec2 pos;\n"
					"out vec2 uv;\n"
					"void main() {\n"
					"  gl_Position = vec4((pos.xy - 0.5) * 2.0, 0.0, 1.0);\n"
					"  uv = vec2(pos.x, 1.0 - pos.y);\n"
					"}\n";
			display_fs_src =
					"#version 330\n"
					"uniform sampler2D tex;\n"
					"in vec2 uv;\n"
					"out vec4 frag_color;\n"
					"void main() {\n"
					"  vec4 texel = texture(tex, uv);\n"
					"  frag_color = texel;\n"
					"}\n";
			break;
		case SG_BACKEND_GLES3:
			display_vs_src =
					"attribute vec2 pos;\n"
					"varying vec2 uv;\n"
					"void main() {\n"
					"  gl_Position = vec4((pos.xy - 0.5) * 2.0, 0.0, 1.0);\n"
					"  uv = vec2(pos.x, 1.0 - pos.y);\n"
					"}\n";
			display_fs_src =
					"precision mediump float;\n"
					"uniform sampler2D tex;\n"
					"varying vec2 uv;\n"
					"void main() {\n"
					"  vec4 texel = texture(tex, uv);\n"
					"  frag_color = texel;\n"
					"}\n";
			break;
		default:
			break;
	}

	sg_shader_desc shd_desc = (sg_shader_desc) {
			.vs = {
					.source = display_vs_src,
			},
			.fs = {
					.source = display_fs_src,
					.images = {
							[0] = {.used = true, .image_type = SG_IMAGETYPE_2D},
					},
					.samplers = {
							[0] = {.used = true, .sampler_type = SG_SAMPLERTYPE_FILTERING},
					},
					.image_sampler_pairs = {
							[0] = {
									.used = true,
									.glsl_name = "tex",
									.image_slot = 0,
									.sampler_slot = 0,
							},
					},
			},
	};

	/* images and samplers */
	const int screen_width = _pti.desc.window.width;
	const int screen_height = _pti.desc.window.height;
	sg_image img = sg_make_image(&(sg_image_desc) {
			.render_target = false,
			.width = screen_width,
			.height = screen_height,
			.pixel_format = SG_PIXELFORMAT_RGBA8,
			.usage = SG_USAGE_STREAM,
	});

	sg_sampler smp = sg_make_sampler(&(sg_sampler_desc) {
			.min_filter = SG_FILTER_NEAREST,
			.mag_filter = SG_FILTER_NEAREST,
			.wrap_u = SG_WRAP_CLAMP_TO_EDGE,
			.wrap_v = SG_WRAP_CLAMP_TO_EDGE,
			.label = "screen-sampler",
	});

	/* default pass action */
	state.display.pass_action = (sg_pass_action) {
			.colors[0] = {
					.load_action = SG_LOADACTION_CLEAR,
					.clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
			},
	};

	state.display.pip = sg_make_pipeline(&(sg_pipeline_desc) {
			.shader = sg_make_shader(&shd_desc),
			.index_type = SG_INDEXTYPE_UINT16,
			.layout = {
					.attrs = {
							[0].format = SG_VERTEXFORMAT_FLOAT2,
					},
			},
			.label = "quad-pipeline",
	});

	/* bindings */
	state.display.bind = (sg_bindings) {
			.vertex_buffers[0] = vbuf,
			.index_buffer = ibuf,
			.fs = {
					.images[0] = img,
					.samplers[0] = smp,
			},
	};
}

void sokol_gfx_draw(uint32_t *ptr) {
	/* update image data */
	const int screen_w = _pti.vm.screen.width;
	const int screen_h = _pti.vm.screen.height;
	const size_t size = screen_w * screen_h * sizeof(uint32_t);
	sg_update_image(state.display.bind.fs.images[0],
					&(sg_image_data) {
							.subimage[0][0] = (sg_range) {
									.ptr = ptr,
									.size = size,
							},
					});

	sg_begin_pass(&(sg_pass) {.action = state.display.pass_action, .swapchain = sglue_swapchain()});
	sg_apply_pipeline(state.display.pip);
	sg_apply_bindings(&state.display.bind);
	sg_draw(0, 6, 1);
	sg_end_pass();
	sg_commit();
}

static void init(void) {
	/* initialize graphics */
	sokol_init_gfx();

	/* initialize game */
	// void *init_cb = _pti.desc.init_cb;
	// if (init_cb != NULL) {
	//   init_cb();
	// }
	_pti.desc.init_cb();
}

static void cleanup(void) {}

static void frame(void) {
	// run the game at a fixed tick rate regardless of frame rate
	uint32_t frame_time_ns = (uint32_t) (sapp_frame_duration() * 1000000000.0);
	// clamp max frame time (so the timing isn't messed up when stopping in the
	// debugger)
	if (frame_time_ns > 33333333) {
		frame_time_ns = PTI_DELTA * 1000000000;
	}

	state.timing.tick_accum += frame_time_ns;
	while (state.timing.tick_accum > -TICK_TOLERANCE_NS) {
		state.timing.tick_accum -= TICK_DURATION_NS;
		state.timing.tick++;

		// call per-tick sound function (updates sound 'registers' with current
		// sound effect values)
		_pti.desc.frame_cb();

		for (int i = 0; i < PTI_BUTTON_COUNT; i++) {
			_pti.vm.hardware.btn_state[i] &= ~(_PTI_KEY_PRESSED | _PTI_KEY_RELEASED);
		}
	}

	/* draw graphics */
	sokol_gfx_draw(_pti.screen);
}

static void input(const sapp_event *ev) {
#define BTN_DOWN(pti_key, sapp_key, sapp_alt)                                       \
	if (ev->key_code == sapp_key || ev->key_code == sapp_alt) {                     \
		_pti.vm.hardware.btn_state[pti_key] |= (_PTI_KEY_STATE | _PTI_KEY_PRESSED); \
		_pti.vm.hardware.btn_state[pti_key] &= ~_PTI_KEY_RELEASED;                  \
	}

#define BTN_UP(pti_key, sapp_key, sapp_alt)                                          \
	if (ev->key_code == sapp_key || ev->key_code == sapp_alt) {                      \
		_pti.vm.hardware.btn_state[pti_key] &= ~(_PTI_KEY_STATE | _PTI_KEY_PRESSED); \
		_pti.vm.hardware.btn_state[pti_key] |= _PTI_KEY_RELEASED;                    \
	}

	switch (ev->type) {
		// keyboard:
		case SAPP_EVENTTYPE_KEY_DOWN:
			if (ev->key_repeat) {
				break;
			}
			BTN_DOWN(PTI_RIGHT, SAPP_KEYCODE_RIGHT, SAPP_KEYCODE_D);
			BTN_DOWN(PTI_LEFT, SAPP_KEYCODE_LEFT, SAPP_KEYCODE_A);
			BTN_DOWN(PTI_UP, SAPP_KEYCODE_UP, SAPP_KEYCODE_W);
			BTN_DOWN(PTI_DOWN, SAPP_KEYCODE_DOWN, SAPP_KEYCODE_S);
			BTN_DOWN(PTI_A, SAPP_KEYCODE_Z, SAPP_KEYCODE_Z);
			BTN_DOWN(PTI_B, SAPP_KEYCODE_X, SAPP_KEYCODE_X);
			BTN_DOWN(PTI_DBG, SAPP_KEYCODE_C, SAPP_KEYCODE_C);

			break;
		case SAPP_EVENTTYPE_KEY_UP:
			BTN_UP(PTI_RIGHT, SAPP_KEYCODE_RIGHT, SAPP_KEYCODE_D);
			BTN_UP(PTI_LEFT, SAPP_KEYCODE_LEFT, SAPP_KEYCODE_A);
			BTN_UP(PTI_UP, SAPP_KEYCODE_UP, SAPP_KEYCODE_W);
			BTN_UP(PTI_DOWN, SAPP_KEYCODE_DOWN, SAPP_KEYCODE_S);
			BTN_UP(PTI_A, SAPP_KEYCODE_Z, SAPP_KEYCODE_Z);
			BTN_UP(PTI_B, SAPP_KEYCODE_X, SAPP_KEYCODE_X);
			BTN_UP(PTI_DBG, SAPP_KEYCODE_C, SAPP_KEYCODE_C);
			break;

		// mouse:
		case SAPP_EVENTTYPE_MOUSE_MOVE:
			break;

		default:
			break;
	}

#undef BTN_DOWN
#undef BTN_UP
}