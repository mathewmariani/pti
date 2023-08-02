#include "content.h"
#include "assets/sprite.h"
#include "blah/include/blah_spatial.h"
#include "game.h"
#include <stdlib.h>
#include <vector>

using namespace TL;

#define ASE_MS(ms) (ms / 1000.0f)

namespace {
const std::vector<Sprite>
    sprites = {{.name = "player",
                .origin = Vec2f(20.0f, 28.0f),
                .animations =
                    {
                        {.name = "sword",
                         .frames = {{
                             .image =
                                 {
                                     .n = 400,
                                     .w = 5,
                                     .h = 5,
                                 },
                             .duration = 100.0f,
                         }}},
                        {.name = "idle",
                         .frames = {{
                                        .image =
                                            {
                                                .n = 405,
                                                .w = 5,
                                                .h = 5,
                                            },
                                        .duration = ASE_MS(300),
                                    },
                                    {
                                        .image =
                                            {
                                                .n = 410,
                                                .w = 5,
                                                .h = 5,
                                            },
                                        .duration = ASE_MS(200),
                                    },
                                    {
                                        .image =
                                            {
                                                .n = 415,
                                                .w = 5,
                                                .h = 5,
                                            },
                                        .duration = ASE_MS(200),
                                    }}},
                        {.name = "jump",
                         .frames =
                             {
                                 {
                                     .image =
                                         {
                                             .n = 420,
                                             .w = 5,
                                             .h = 5,
                                         },
                                     .duration = ASE_MS(200),
                                 },
                             }},
                        {.name = "run",
                         .frames =
                             {
                                 {
                                     .image =
                                         {
                                             .n = 420,
                                             .w = 5,
                                             .h = 5,
                                         },
                                     .duration = ASE_MS(200),
                                 },
                                 {
                                     .image =
                                         {
                                             .n = 425,
                                             .w = 5,
                                             .h = 5,
                                         },
                                     .duration = ASE_MS(200),
                                 },
                             }},
                        {.name = "attack",
                         .frames =
                             {
                                 {
                                     .image =
                                         {
                                             .n = 430,
                                             .w = 5,
                                             .h = 5,
                                         },
                                     .duration = ASE_MS(200),
                                 },
                                 {
                                     .image =
                                         {
                                             .n = 435,
                                             .w = 5,
                                             .h = 5,
                                         },
                                     .duration = ASE_MS(300),
                                 },
                                 {
                                     .image =
                                         {
                                             .n = 440,
                                             .w = 5,
                                             .h = 5,
                                         },
                                     .duration = ASE_MS(100),
                                 },
                             }},
                        {.name = "hurt",
                         .frames =
                             {
                                 {
                                     .image =
                                         {
                                             .n = 445,
                                             .w = 5,
                                             .h = 5,
                                         },
                                     .duration = ASE_MS(100),
                                 },
                             }},
                    }},
               {
                   .name = "bramble",
                   .origin = Vec2f(8.0f, 16.0f),
                   .animations = {{.name = "idle",
                                   .frames =
                                       {
                                           {
                                               .image =
                                                   {
                                                       .n = 200,
                                                       .w = 2,
                                                       .h = 2,
                                                   },
                                               .duration = ASE_MS(100),
                                           }}}},
               },
               {
                   .name = "blob",
                   .origin = Vec2f(8.0f, 16.0f),
                   .animations = {{.name = "idle",
                                   .frames = {{
                                                  .image =
                                                      {
                                                          .n = 208,
                                                          .w = 2,
                                                          .h = 2,
                                                      },
                                                  .duration = ASE_MS(250),
                                              },
                                              {
                                                  .image =
                                                      {
                                                          .n = 210,
                                                          .w = 2,
                                                          .h = 2,
                                                      },
                                                  .duration = ASE_MS(250),
                                              }}},
                                  {.name = "jump",
                                   .frames =
                                       {
                                           {
                                               .image =
                                                   {
                                                       .n = 212,
                                                       .w = 2,
                                                       .h = 2,
                                                   },
                                               .duration = ASE_MS(250),
                                           }}}},
               },
               {
                   .name = "pop",
                   .origin = Vec2f(12.0f, 12.0f),
                   .animations = {{.name = "pop",
                                   .frames = {{
                                                  .image =
                                                      {
                                                          .n = 650,
                                                          .w = 3,
                                                          .h = 3,
                                                      },
                                                  .duration = ASE_MS(100),
                                              },
                                              {
                                                  .image =
                                                      {
                                                          .n = 653,
                                                          .w = 3,
                                                          .h = 3,
                                                      },
                                                  .duration = ASE_MS(50),
                                              },
                                              {
                                                  .image =
                                                      {
                                                          .n = 656,
                                                          .w = 3,
                                                          .h = 3,
                                                      },
                                                  .duration = ASE_MS(50),
                                              },
                                              {
                                                  .image =
                                                      {
                                                          .n = 659,
                                                          .w = 3,
                                                          .h = 3,
                                                      },
                                                  .duration = ASE_MS(50),
                                              },
                                              {
                                                  .image =
                                                      {
                                                          .n = 662,
                                                          .w = 3,
                                                          .h = 3,
                                                      },
                                                  .duration = ASE_MS(50),
                                              },
                                              {
                                                  .image =
                                                      {
                                                          .n = 665,
                                                          .w = 3,
                                                          .h = 3,
                                                      },
                                                  .duration = ASE_MS(50),
                                              }}}},
               },
               {
                   .name = "heart",
                   .origin = Vec2f(8.0f, 16.0f),
                   .animations = {{.name = "full",
                                   .frames =
                                       {
                                           {
                                               .image =
                                                   {
                                                       .n = 202,
                                                       .w = 2,
                                                       .h = 2,
                                                   },
                                               .duration = ASE_MS(100),
                                           }}},
                                  {.name = "empty",
                                   .frames =
                                       {
                                           {
                                               .image =
                                                   {
                                                       .n = 204,
                                                       .w = 2,
                                                       .h = 2,
                                                   },
                                               .duration = ASE_MS(100),
                                           }}}},
               },
               {
                   .name = "bullet",
                   .origin = Vec2f(8.0f, 8.0f),
                   .animations =
                       {
                           {.name = "idle",
                            .frames =
                                {
                                    {
                                        .image =
                                            {
                                                .n = 206,
                                                .w = 2,
                                                .h = 2,
                                            },
                                        .duration = ASE_MS(100),
                                    }}},
                       },
               },
               {
                   .name = "door",
                   .origin = Vec2f(8.0f, 16.0f),
                   .animations =
                       {
                           {.name = "idle",
                            .frames =
                                {
                                    {
                                        .image =
                                            {
                                                .n = 300,
                                                .w = 2,
                                                .h = 2,
                                            },
                                        .duration = ASE_MS(100),
                                    }}},
                       },
               },
               {
                   .name = "mosquito",
                   .origin = Vec2f(8.0f, 5.0f),
                   .animations =
                       {
                           {.name = "fly",
                            .frames = {{
                                           .image =
                                               {
                                                   .n = 302,
                                                   .w = 2,
                                                   .h = 2,
                                               },
                                           .duration = ASE_MS(100),
                                       },
                                       {
                                           .image =
                                               {
                                                   .n = 304,
                                                   .w = 2,
                                                   .h = 2,
                                               },
                                           .duration = ASE_MS(100),
                                       }}},
                       },
               },
               {
                   .name = "spitter",
                   .origin = Vec2f(14.0f, 24.0f),
                   .animations = {{.name = "idle",
                                   .frames = {{
                                                  .image =
                                                      {
                                                          .n = 8,
                                                          .w = 3,
                                                          .h = 3,
                                                      },
                                                  .duration = ASE_MS(200),
                                              },
                                              {
                                                  .image =
                                                      {
                                                          .n = 11,
                                                          .w = 3,
                                                          .h = 3,
                                                      },
                                                  .duration = ASE_MS(200),
                                              },
                                              {
                                                  .image =
                                                      {
                                                          .n = 14,
                                                          .w = 3,
                                                          .h = 3,
                                                      },
                                                  .duration = ASE_MS(200),
                                              }}},
                                  {.name = "shoot",
                                   .frames =
                                       {
                                           {
                                               .image =
                                                   {
                                                       .n = 17,
                                                       .w = 3,
                                                       .h = 3,
                                                   },
                                               .duration = ASE_MS(400),
                                           }}}},
               }};
} // namespace

const Sprite *Content::find_sprite(const std::string &name) {
  for (auto &it : sprites)
    if (it.name == name)
      return &it;

  return &sprites[0];
}
