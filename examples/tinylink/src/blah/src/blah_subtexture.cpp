#include "../include/blah_subtexture.h"
#include "../include/blah_calc.h"

using namespace Blah;

Subtexture::Subtexture() : Subtexture(Rectf(0, 0, 64, 64)) {}

Subtexture::Subtexture(Rectf source)
    : Subtexture(source, Rectf(0, 0, source.w, source.h)) {}

Subtexture::Subtexture(Rectf source, Rectf frame)
    : source(source), frame(frame) {
  update();
}

void Subtexture::update() {
  draw_coords[0].x = -frame.x;
  draw_coords[0].y = -frame.y;
  draw_coords[1].x = -frame.x + source.w;
  draw_coords[1].y = -frame.y;
  draw_coords[2].x = -frame.x + source.w;
  draw_coords[2].y = -frame.y + source.h;
  draw_coords[3].x = -frame.x;
  draw_coords[3].y = -frame.y + source.h;
}

void Subtexture::crop_info(const Rectf &clip, Rectf *dest_source,
                           Rectf *dest_frame) const {
  *dest_source =
      (clip + source.top_left() + frame.top_left()).overlap_rect(source);

  dest_frame->x = Calc::min(0.0f, frame.x + clip.x);
  dest_frame->y = Calc::min(0.0f, frame.y + clip.y);
  dest_frame->w = clip.w;
  dest_frame->h = clip.h;
}

Subtexture Subtexture::crop(const Rectf &clip) const {
  Subtexture dst;
  // dst.texture = texture;
  // crop_info(clip, &dst.source, &dst.frame);
  // dst.update();
  return dst;
}
