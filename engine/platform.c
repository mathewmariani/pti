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

static struct {
	struct {
		unsigned int tick;
		int tick_accum;
	} timing;

	struct {
		sg_pass pass;
		sg_pipeline pipeline;
		sg_bindings binding;
		sg_image target;
	} gfx;
} state;


static void sokol_init_gfx(void) {
	/* setup sokol-gfx */
	sg_setup(&(sg_desc) {
			.environment = sglue_environment(),
			.buffer_pool_size = 2,
			.image_pool_size = 1,
			.shader_pool_size = 1,
			.pipeline_pool_size = 1,
			.logger.func = slog_func,
	});

	const char *display_vs_src = 0;
	const char *display_fs_src = 0;
	switch (sg_query_backend()) {
		case SG_BACKEND_GLCORE:
			display_vs_src =
					"#version 410\n"
					"layout(location=0) in vec2 pos;\n"
					"out vec2 uv;\n"
					"void main() {\n"
					"  gl_Position = vec4((pos.xy - 0.5) * 2.0, 0.0, 1.0);\n"
					"  uv = vec2(pos.x, 1.0 - pos.y);\n"
					"}\n";
			display_fs_src =
					"#version 410\n"
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
					"#version 300 es\n"
					"attribute vec2 pos;\n"
					"varying vec2 uv;\n"
					"void main() {\n"
					"  gl_Position = vec4((pos.xy - 0.5) * 2.0, 0.0, 1.0);\n"
					"  uv = vec2(pos.x, 1.0 - pos.y);\n"
					"}\n";
			display_fs_src =
					"#version 300 es\n"
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

	state.gfx.pass = (sg_pass) {
			.action = (sg_pass_action) {
					.colors[0] = {
							.load_action = SG_LOADACTION_CLEAR,
							.clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
					},
			},
			.swapchain = sglue_swapchain(),
	};

	state.gfx.pipeline = sg_make_pipeline(&(sg_pipeline_desc) {
			.shader = sg_make_shader(&(sg_shader_desc) {
					.vertex_func.source = display_vs_src,
					.fragment_func.source = display_fs_src,
					.images[0] = {.stage = SG_SHADERSTAGE_FRAGMENT, .image_type = SG_IMAGETYPE_2D},
					.samplers[0] = {.stage = SG_SHADERSTAGE_FRAGMENT, .sampler_type = SG_SAMPLERTYPE_FILTERING},
					.image_sampler_pairs[0] = {
							.stage = SG_SHADERSTAGE_FRAGMENT,
							.glsl_name = "tex",
							.image_slot = 0,
							.sampler_slot = 0,
					},
			}),
			.index_type = SG_INDEXTYPE_UINT16,
			.layout = {
					.attrs = {
							[0].format = SG_VERTEXFORMAT_FLOAT2,
					},
			},
			.label = "quad-pipeline",
	});

	const float vertices[] = {
			-1.0f, 1.0f,  /* top-left */
			1.0f, 1.0f,   /* top-right */
			1.0f, -1.0f,  /* bottom-right */
			-1.0f, -1.0f, /* bottom-left */
	};
	const uint16_t indices[] = {0, 1, 2, 0, 2, 3};

	/* bindings */
	state.gfx.binding = (sg_bindings) {
			.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
					.data = SG_RANGE(vertices),
					.label = "quad-vertices",
			}),
			.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
					.type = SG_BUFFERTYPE_INDEXBUFFER,
					.data = SG_RANGE(indices),
					.label = "quad-indices",
			}),
			.images[0] = sg_make_image(&(sg_image_desc) {
					.render_target = false,
					.width = _pti.desc.window.width,
					.height = _pti.desc.window.height,
					.pixel_format = SG_PIXELFORMAT_RGBA8,
					.usage = SG_USAGE_STREAM,
					.label = "screen-image",
			}),
			.samplers[0] = sg_make_sampler(&(sg_sampler_desc) {
					.min_filter = SG_FILTER_NEAREST,
					.mag_filter = SG_FILTER_NEAREST,
					.wrap_u = SG_WRAP_CLAMP_TO_EDGE,
					.wrap_v = SG_WRAP_CLAMP_TO_EDGE,
					.label = "screen-sampler",
			}),
	};
}

void sokol_gfx_draw() {
	/* update image data */
	const int screen_w = _pti.vm.screen.width;
	const int screen_h = _pti.vm.screen.height;
	const size_t size = screen_w * screen_h * sizeof(uint32_t);
	sg_update_image(state.gfx.binding.images[0],
					&(sg_image_data) {
							.subimage[0][0] = (sg_range) {
									.ptr = _pti.screen,
									.size = size,
							},
					});

	sg_begin_pass(&state.gfx.pass);
	sg_apply_pipeline(state.gfx.pipeline);
	sg_apply_bindings(&state.gfx.binding);
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

#define TICK_DURATION_NS (PTI_DELTA * 1e9)
#define TICK_TOLERANCE_NS (1000000)

static void frame(void) {
	uint32_t frame_time_ns = (uint32_t) (sapp_frame_duration() * 1000000000.0);
	if (frame_time_ns > TICK_DURATION_NS) {
		frame_time_ns = TICK_DURATION_NS;
	}

	state.timing.tick_accum += frame_time_ns;
	while (state.timing.tick_accum > -TICK_TOLERANCE_NS) {
		state.timing.tick_accum -= TICK_DURATION_NS;
		state.timing.tick++;

		_pti.desc.frame_cb();

		for (int i = 0; i < PTI_BUTTON_COUNT; i++) {
			_pti.vm.hardware.btn_state[i] &= ~(_PTI_KEY_PRESSED | _PTI_KEY_RELEASED);
		}
	}

	/* draw graphics */
	sokol_gfx_draw();
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