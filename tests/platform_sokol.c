/* sokol */
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_log.h"

#define PTI_IMPL
#include "pti.h"

/* forward declarations */
static void init(void);
static void frame(void);
static void cleanup(void);
static void event(const sapp_event *);

/* main entry point */
sapp_desc sokol_main(int argc, char *argv[]) {
	(void) argc;
	(void) argv;

	pti_desc desc = pti_main(argc, argv);
	pti_init(&desc);

	int width = desc.width * 3;
	int height = desc.height * 3;

	return (sapp_desc) {
			.init_cb = init,
			.frame_cb = frame,
			.cleanup_cb = cleanup,
			.event_cb = event,
			.width = width,
			.height = height,
			.window_title = "pti (sokol)",
			.logger = {
					.func = slog_func,
			},
#if defined(SOKOL_GLCORE)
#if defined(__APPLE__)
			.gl = {
					.major_version = 4,
					.minor_version = 1,
			}
#else
			.gl = {
					.major_version = 4,
					.minor_version = 2,
			}
#endif
#endif
	};
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

	const char *display_vs_src =
#if defined(SOKOL_GLCORE)
			"#version 410\n"
#elif defined(SOKOL_GLES3)
			"#version 300 es\n"
			"precision mediump float;\n"
#endif
			"layout(location=0) in vec2 pos;\n"
			"out vec2 vs_texcoord;\n"
			"void main() {\n"
			"  gl_Position = vec4((pos.xy - 0.5) * 2.0, 0.0, 1.0);\n"
			"  vs_texcoord = vec2(pos.x, 1.0 - pos.y);\n"
			"}\n";

	const char *display_fs_src =
#if defined(SOKOL_GLCORE)
			"#version 410\n"
#elif defined(SOKOL_GLES3)
			"#version 300 es\n"
			"precision mediump float;\n"
			"precision mediump sampler2D;\n"
#endif
			"float PI = 3.14159;\n"
			"uniform sampler2D screen;\n"
			"in vec2 vs_texcoord;\n"
			"out vec4 frag_color;\n"
			"vec2 curvature = vec2(10.0);\n"
			"vec2 scanLineOpacity = vec2(0.5);\n"
			"float brightness = 2.0;\n"
			"float vignetteOpacity = 0.5;\n"
			"float vignetteRoundness = 1.0;\n"
			"vec2 curveRemapUV(vec2 uv) {\n"
			"  uv = uv * 2.0 - 1.0;\n"
			"  vec2 offset = abs(uv.yx) / vec2(curvature.x, curvature.y);\n"
			"  uv = uv + uv * offset * offset;\n"
			"  uv = uv * 0.5 + 0.5;\n"
			"  return uv;\n"
			"}\n"
			"vec3 scanLineIntensity(float uv, float resolution, float opacity) {\n"
			"  float intensity = sin(uv * resolution * PI * 2.0);\n"
			"  intensity = ((0.5 * intensity) + 0.5) * 0.9 + 0.1;\n"
			"  return vec3(pow(intensity, opacity));\n"
			"}\n"
			"vec3 vignetteIntensity(vec2 uv, vec2 resolution, float opacity, float roundness) {\n"
			"  float intensity = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);\n"
			"  return vec3(clamp(pow((resolution.x / roundness) * intensity, opacity), 0.0, 1.0));\n"
			"}\n"
			"void main() {\n"
			"  vec2 uv = curveRemapUV(vs_texcoord);\n"
			"  if (uv.x < 0.0 || uv.y < 0.0 || uv.x > 1.0 || uv.y > 1.0) {\n"
			"    frag_color = vec4(0.0, 0.0, 0.0, 1.0);\n"
			"    return;\n"
			"  }\n"
			"  vec3 base_color = texture(screen, uv).rgb;\n"
			"  vec2 resolution = vec2(textureSize(screen, 0));\n"
			"  base_color *= vignetteIntensity(uv, resolution, vignetteOpacity, vignetteRoundness);\n"
			"  base_color *= scanLineIntensity(uv.x, resolution.x, scanLineOpacity.x);\n"
			"  base_color *= scanLineIntensity(uv.y, resolution.y, scanLineOpacity.y);\n"
			"  base_color *= brightness;\n"
			"  frag_color = vec4(base_color, 1.0);\n"
			"}\n";

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
					.views = {
							[0].texture = {.stage = SG_SHADERSTAGE_FRAGMENT, .wgsl_group1_binding_n = 0},
					},
					.samplers[0] = {.stage = SG_SHADERSTAGE_FRAGMENT, .sampler_type = SG_SAMPLERTYPE_FILTERING},
					.texture_sampler_pairs[0] = {
							.stage = SG_SHADERSTAGE_FRAGMENT,
							.glsl_name = "screen",
							.view_slot = 0,
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

	// clang-format off
	const float vertices[] = {
			-1.0f, 1.0f,  /* top-left */
			1.0f, 1.0f,   /* top-right */
			1.0f, -1.0f,  /* bottom-right */
			-1.0f, -1.0f, /* bottom-left */
	};
	// clang-format on
	const uint16_t indices[] = {0, 1, 2, 0, 2, 3};

	/* bindings */
	state.gfx.target = sg_make_image(&(sg_image_desc) {
			.width = _pti.desc.width,
			.height = _pti.desc.height,
			.pixel_format = SG_PIXELFORMAT_RGBA8,
			.usage = {.stream_update = true},
			.label = "screen-image",
	});
	state.gfx.binding = (sg_bindings) {
			.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
					.usage = {
							.vertex_buffer = true,
					},
					.data = SG_RANGE(vertices),
					.label = "quad-vertices",
			}),
			.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
					.usage = {
							.index_buffer = true,
					},
					.data = SG_RANGE(indices),
					.label = "quad-indices",
			}),
			.views[0] = sg_make_view(&(sg_view_desc) {
					.texture.image = state.gfx.target,
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

	sg_update_image(state.gfx.target, &(sg_image_data) {
											  .mip_levels[0] = (sg_range) {
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
	if (_pti.desc.init_cb != NULL) {
		_pti.desc.init_cb();
	}
}

static void cleanup(void) {
	sg_destroy_pipeline(state.gfx.pipeline);
	sg_destroy_image(state.gfx.target);
	sg_destroy_buffer(state.gfx.binding.vertex_buffers[0]);
	sg_destroy_buffer(state.gfx.binding.index_buffer);
	sg_destroy_sampler(state.gfx.binding.samplers[0]);
	sg_shutdown();
}

static void frame(void) {
	if (_pti.vm.interrupts == PTI_REQUEST_SHUTDOWN) {
		sapp_request_quit();
		return;
	}

	double frame_time_ns = sapp_frame_duration() * 1000000000.0;
	pti_tick(frame_time_ns);

	/* draw graphics */
	sokol_gfx_draw();

#if defined(PTI_DEBUG)
	/* debug ui */
	__dbgui_begin();
	imgui_debug_draw();
	if (_pti.desc.debug_cb != NULL) {
		_pti.desc.debug_cb();
	}
	__dbgui_end();
#endif
}
static inline void btn_down(int pti_key, int sapp_key, int sapp_alt, const sapp_event *ev) {
	if (ev->key_code == sapp_key || ev->key_code == sapp_alt) {
		pti_event(PTI_EVENTTYPE_KEY_DOWN, pti_key);
	}
}

static inline void btn_up(int pti_key, int sapp_key, int sapp_alt, const sapp_event *ev) {
	if (ev->key_code == sapp_key || ev->key_code == sapp_alt) {
		pti_event(PTI_EVENTTYPE_KEY_UP, pti_key);
	}
}

static void event(const sapp_event *ev) {
	switch (ev->type) {
		/* keyboard: */
		case SAPP_EVENTTYPE_KEY_DOWN:
			if (ev->key_repeat) {
				break;
			}
			btn_down(PTI_RIGHT, SAPP_KEYCODE_RIGHT, SAPP_KEYCODE_RIGHT, ev);
			btn_down(PTI_LEFT, SAPP_KEYCODE_LEFT, SAPP_KEYCODE_LEFT, ev);
			btn_down(PTI_UP, SAPP_KEYCODE_UP, SAPP_KEYCODE_UP, ev);
			btn_down(PTI_DOWN, SAPP_KEYCODE_DOWN, SAPP_KEYCODE_DOWN, ev);
			btn_down(PTI_A, SAPP_KEYCODE_D, SAPP_KEYCODE_D, ev);
			btn_down(PTI_B, SAPP_KEYCODE_S, SAPP_KEYCODE_S, ev);
			btn_down(PTI_X, SAPP_KEYCODE_W, SAPP_KEYCODE_W, ev);
			btn_down(PTI_Y, SAPP_KEYCODE_A, SAPP_KEYCODE_A, ev);
			btn_down(PTI_LSHOULDER, SAPP_KEYCODE_Q, SAPP_KEYCODE_Q, ev);
			btn_down(PTI_RSHOULDER, SAPP_KEYCODE_E, SAPP_KEYCODE_E, ev);
			btn_down(PTI_START, SAPP_KEYCODE_X, SAPP_KEYCODE_X, ev);
			btn_down(PTI_SELECT, SAPP_KEYCODE_Z, SAPP_KEYCODE_Z, ev);
			btn_down(PTI_DBG, SAPP_KEYCODE_C, SAPP_KEYCODE_C, ev);
			break;

		case SAPP_EVENTTYPE_KEY_UP:
			btn_up(PTI_RIGHT, SAPP_KEYCODE_RIGHT, SAPP_KEYCODE_RIGHT, ev);
			btn_up(PTI_LEFT, SAPP_KEYCODE_LEFT, SAPP_KEYCODE_LEFT, ev);
			btn_up(PTI_UP, SAPP_KEYCODE_UP, SAPP_KEYCODE_UP, ev);
			btn_up(PTI_DOWN, SAPP_KEYCODE_DOWN, SAPP_KEYCODE_DOWN, ev);
			btn_up(PTI_A, SAPP_KEYCODE_D, SAPP_KEYCODE_D, ev);
			btn_up(PTI_B, SAPP_KEYCODE_S, SAPP_KEYCODE_S, ev);
			btn_up(PTI_X, SAPP_KEYCODE_W, SAPP_KEYCODE_W, ev);
			btn_up(PTI_Y, SAPP_KEYCODE_A, SAPP_KEYCODE_A, ev);
			btn_up(PTI_LSHOULDER, SAPP_KEYCODE_Q, SAPP_KEYCODE_Q, ev);
			btn_up(PTI_RSHOULDER, SAPP_KEYCODE_E, SAPP_KEYCODE_E, ev);
			btn_up(PTI_START, SAPP_KEYCODE_X, SAPP_KEYCODE_X, ev);
			btn_up(PTI_SELECT, SAPP_KEYCODE_Z, SAPP_KEYCODE_Z, ev);
			btn_up(PTI_DBG, SAPP_KEYCODE_C, SAPP_KEYCODE_C, ev);
			break;

		/* mouse: */
		case SAPP_EVENTTYPE_MOUSE_MOVE:
			break;

		default:
			break;
	}
}