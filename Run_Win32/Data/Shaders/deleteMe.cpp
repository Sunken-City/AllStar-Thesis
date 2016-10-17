/*
Lectures Outline


- End Goal
-- What is Renderer (or at least, how I'm going to define it)
-- Virtual Coordinate Systems
-- RenderLoop of the SpriteGameRenderer
-- Shaders for 2D Games
- Refresher Matrix Math (2D Emphasis)
- Constructing the Sprite
- Post Process Pipeline

// Whatever other questions come up.
- Generalizing as a Surface
- Fixed Frame



// What we did in SD3
/* Lighting */

// Sprites
// SpriteResources
// Order Layers
// Full Screen Effects

// Positioning (Camera System in 2D)

//Everything static about the sprite, nothing instanced

class SpriteResource
{
  public:
    Texture2D *texture;
    vec2 uv_top_left;
    vec2 uv_top_right;

    vec2 pixel_size;
    vec2 virtual_size;

    vec2 pivot_point; // Center of Rotation and Scale (basically the origin of this sprite in local space)

    RMaterial *default_material;
};

// AKA - A SpriteInstance
class Sprite
{
  public:
    SpriteResource const *sprite_resource;

    int ordering_layer; // drawing order is ordered by layer, smallest to largest

    vec2 position;
    vec2 scale;
    float rotation;

    color tint;

    bool enabled; // If disabled - does not get rendered

    RMaterial *material;

    Sprite *prev, *next;


    void SetLayer( int layer )
    {
      if (enabled) {
        SpriteGameRenderer::Instance()->UnregisterSprite( this );
      }

      ordering_layer = layer;
      if (enabled) {
        SpriteGameRenderer::Instance()->RegisterSprite( this );
      }
    }

    void Enable()
    {
      if (!enabled) {
        SpriteGameRenderer::Instance()->RegisterSprite( this );
        enabled = true;
      }
    }

    void Disable()
    {
      if (enabled) {
        SpriteGameRenderer::Instance()->UnregisterSprite( this );
        enabled = false;
      }
    }
};

int const BACKGROUND_LAYER = 0;
int const ENEMY_LAYER = 9;
int const PLAYER_LAYER = 10;
int const PLAYER_BULLET_LAYER = 11;
int const ENEMY_BULLET_LAYER = 12;
int const FOREGROUND_LAYER = 20;
int const UI_LAYER = 30;


#define BIT_FLAG(f) (1 << (f))

enum class eAlignment
{
  CENTER = 0,
  LEFT = BIT_FLAG(1),
  RIGHT = BIT_FLAG(2),
  TOP = BIT_FLAG(3),
  LEFT = BIT_FLAG(4),

  TOP_LEFT = TOP | LEFT,
  TOP_RIGHT = TOP | RIGHT,
  BOTTOM_LEFT = BOTTOM | LEFT,
  BOTTOM_RIGHT = BOTTOM | RIGHT,
};

template <typename T>
void AddInPlace( T *&list, T *sprite )
{
  if (list == nullptr) {
    list = sprite;
    list->next = list->prev = list;
  } else {
    sprite->prev = list->prev;
    sprite->next = list;
    list->prev->next = sprite;
    list->prev = sprite;
  }
}

void RemoveInPlace( T *&list, T *sprite )
{
  // make sure this will set list back to null if sprite was the only element in it.
  // "Trivial and left as an excercise for the reader."
}

//
//
//
class SpriteLayer
{
  public:
    int layer;
    Sprite *sprite_list;

    // vector<FullScreenEffects*> current_effects;

    void AddSprite( Sprite *sprite )
    {
      // CONFIRM_IN_LIST( sprite_list, sprite );
      AddInPlace( sprite_list, sprite );
    }

    void RemoveSprite( Sprite *sprite )
    {
      RemoveInPlace( sprite_list, sprite );
    }
};

///
///
///
class SpriteGameRenderer
{
  public:
    void UpdateScreenResolution( uint32_t px_width, uint32_t px_height );

    //The box (Size in game units of our screen)
    void SetVirtualSize( float vsize );

    // What resolution we're authoring at
    //The standardized scale of the sprite in virtual space
    void SetImportSize( float import_size );


    vec2 GetPixelCoordinate( vec2 virtual_coord );

    void SetScissor( aabb2 virtual_bounds );



    vec2 GetVirtualCoordinate( vec2_fl virtual_offset, eAlignment alignment )
    {
      if(alignment == TOP_LEFT)
      {
        vec2_fl startingOffset = (virtual_width / -2.0f, virtual_height / 2.0f);
        return startingOffset + virtual_offset;
      }
    }

    aabb2 GetVirtualBounds() const;

    // Updated when you set VirtualSize and Update your resolution
    float virtual_width;
    float virtual_height;


    void Update( float dt )
    {
      // Does nothing
    }

    void SetClearColor( color c );

    vector<SpriteLayer*> m_layers;

    SpriteLayer* CreateOrFindLayer( int layer_id );
    Material *default_material; //

    //
    Mesh *mesh; // created at init
    MeshRenderer *renderer; // created at init

    void Init()
    {
      default_material = new Material("default.vert", "default.frag");
      mesh = new Mesh();

      mesh->SetIndices( int[] { 0, 1, 2, 2, 1, 3 } );

      renderer = new MeshRenderer( default_material, mesh );

      UpdateProjectionMatrix();
    }

    // Called anytime the resolution or projection matrix changes
    void UpdateProjectMatrix()
    {
      m_proj = MatrixOrthogonal( virtual_width, virtual_height, -1.0f, 1.0f );
    }


    // Copy Sprite to Mesh
    void CopySpriteToMesh( Mesh *mesh, Sprite *sprite )
    {
      sprite_vertex_t verts[4];
      //Calculate the bounding box for our sprite
      // position, scale, rotation, virtual size

      //Scale the bounding box

      //Rotate the bounding box

      //Translate the bounding box

      //Copy the vertices into the mesh

    }

    // Sprite
    void DrawSprite( Sprite *sprite )
    {
      // Early out if setting to same material
      renderer->set_material( sprite->material );

      CopySpriteToMesh( mesh, sprite );

      sprite->material->SetTexture( "gTexDiffuse", sprite->sprite_resource->texture );

      renderer->Render();
    }

    Framebuffer *fb0 = new Framebuffer();
    Framebuffer *fb1 = new Framebuffer();

    Framebuffer *current_fbo = fb0;
    Framebuffer *source_fbo = fb1;


    ////////////////////
    void RenderLayer( SpriteLayer *layer )
    {
      foreach (sprite in layer->sprite_list) {
        DrawSprite(sprite);
      }

      foreach (effect in layer->effects) {
        Bind(source_fbo);
        effect->SetTexture( "gTexInput", current_fbo->GetColorTarget(0) );
        RenderFullScreenEffect( effect );
        Swap( current_fbo, effect_fbo );
      }
    }

    ///////////////////
    void FrameRender()
    {
      ClearColor( m_clear_color );

      foreach (layer in m_layers) {
        RenderLayer(layer);
      }

      FlipsToBack();
    }


};




Game::Init()
{
  SpriteGameRenderer *renderer = SpriteGameRenderer::Instance();
  renderer->init(...);

  renderer->UpdateScreenResolution( 1600, 900 );
  renderer->SetVirtualSize( 15.0f );

  RegisterSprites();

  CreatePlayer();

  // Blur is just FBO style material.. Would recommend
  // extending Material to an FullscreenEffect class.
  // (Just so you know what materials can be used as effects for FBO's)
  blur = new FullscreenEffect( "blur.frag" );
  blur->SetUniformFloat( "gBlurAmount", 4.0f );

  desaturate = new FullscreenEffect( "desaturate.frag" );
  desaturate->SetUniformAlout( "gAmount", .5 );

  // Effect everything
  renderer->AddEffect( BACKGROUND_LAYER, blur );
  renderer->AddEffect( BACKGROUND_LAYER, PLAYER_LAYER );
}

Game::RegisterSprites()
{
  SpriteDatabase *db = SpriteDatabase::Instance();
  db->RegisterSprite( "ship", "sprites/player.png" );
  db->RegisterSprite( "bullet", "sprites/bullet.png" );
  db->RegisterSprite( "enemy", "sprite/blob.png" );
}

struct Player
{
  Sprite *resource;

  float health;
  vec2 position;
};


Game::CreatePlayer()
{
  m_player = new Player();
  m_player->sprite = new Sprite("ship");
  m_player->sprite->SetLayer( PLAYER_LAYER );

  m_player->position = vec2(0.0f);
}

Game::Update( float dt )
{
  SpriteGameRenderer::Instance::Update(dt); // does nothing for now

  // Update Player
  // Could use a single transform to point to and update once.
  m_player->position += dt * vec2( 0, -9.8 );
  m_player->sprite->position = m_player->position;

}

Game::Render()
{
  SpriteGameRenderer::Instance::Render();
}


// 1900x1200
//




//
// SHADERS
//

struct sprite_vertex_t
{
  vec2 position;
  vec2 uv;
  vec4 tint;
};


// VERTEX SHADER
#version 410 core

// uniforms
mat4 gModel;
mat4 gView;
mat4 gProj;

// inputs
in vec2 inPosition;
in vec2 inUV;
in vec4 inTint;

// outputs
out vec2 passUV;
out vec4 passTint;

// main
void main()
{
  mat4 mvp = gModel * gView * gProjection;

  passUV = inUV;
  passTint = inTint;

  // gl_Position is always a vec4 - clip space vector
  gl_Position = vec4( inPosition, 0, 1 ) * mvp;
}


// FRAGMENT SHADER
#version 410 core

// uniforms
uniform sampler2D gTexDiffuse;
uniform vec4 gLayerTint;

// inputs
in vec3 passUV;
in vec4 passTint;

// ouputs
out vec4 fragmentColor;

// main
void main()
{
  vec4 diffuseColor = Texture(gTexDiffuse, passUV);

  fragmentColor = passTint * diffuseColor * gLayerTint;
}
