#include "animator.h"
#include "pti.h"
#include "../content.h"

using namespace TL;

Animator::Animator(const std::string &sprite) {
  m_sprite = Content::find_sprite(sprite);
  m_animation_index = 0;
}

const Sprite *Animator::sprite() const { return m_sprite; }

const Sprite::Animation *Animator::animation() const {
  if (m_sprite && m_animation_index >= 0 &&
      m_animation_index < m_sprite->animations.size())
    return &m_sprite->animations[m_animation_index];
  return nullptr;
}

void Animator::play(const std::string &animation, bool restart) {
  BLAH_ASSERT(m_sprite, "No Sprite Assigned!");

  for (int i = 0; i < m_sprite->animations.size(); i++) {
    if (m_sprite->animations[i].name == animation) {
      if (m_animation_index != i || restart) {
        m_animation_index = i;
        m_frame_index = 0;
        m_frame_counter = 0;
      }

      break;
    }
  }
}

void Animator::update() {
  // only update if we're in a valid state
  if (in_valid_state()) {
    // quick references
    auto &anim = m_sprite->animations[m_animation_index];
    auto &frame = anim.frames[m_frame_index];

    // increment frame counter
    m_frame_counter += Time::delta;

    // move to next frame after duration
    while (m_frame_counter >= frame.duration) {
      // reset frame counter
      m_frame_counter -= frame.duration;

      // increement frame, move back if we're at the end
      m_frame_index++;
      if (m_frame_index >= anim.frames.size())
        m_frame_index = 0;
    }
  }
}

void Animator::render() {
  if (in_valid_state()) {
    const auto x = entity()->position.x + offset.x - m_sprite->origin.x;
    const auto y = entity()->position.y + offset.y - m_sprite->origin.y;

    auto &anim = m_sprite->animations[m_animation_index];
    auto &frame = anim.frames[m_frame_index];

    // pti_rect(x, y, 8, 8, 0xFFFFFFFF);
    const auto n = frame.image.n;
    const auto w = frame.image.w;
    const auto h = frame.image.h;
    pti_sspr(n, x, y, w, h, false, false);
  }
}

bool Animator::in_valid_state() const {
  return m_sprite && m_animation_index >= 0 &&
         m_animation_index < m_sprite->animations.size() &&
         m_frame_index >= 0 &&
         m_frame_index < m_sprite->animations[m_animation_index].frames.size();
}
