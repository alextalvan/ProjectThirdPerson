--[[

API:

  LOADING:
  LoadMesh(string mesh.obj);
  LoadTexture(string texture.jpg);

  CREATION:
  ColorMaterial(color (float r, float g, float b));
  TextureMaterial(texture);
  GameObject(string name, position (float x, float y, float z));
  Camera(string name, position (float x, float y, float z));

  SCENE:
  AddToWorld(world, gameObject);
  SetMainCamera(world, camera);

  BEHAVIOUR:
  KeysBehaviour(float moveSpeed, float turnSpeed)
  OrbitBehaviour(gameObject(target), float radius, float limitX, float limitY, float rotSpeed);
  LookAtBehaviour(gameObject(target));

  TRANSFORMATIONS:
  Rotate(gameObject, float angle, axis (float x, float y, float z));
  Scale(gameObject, scalar (float x, float y, float z));
  Translate(gameObject, translation (float x, float y, float z));

  OBJECT PARAMETERS:
  SetMesh(gameObject, mesh);
  GetMesh(gameObject);
  SetMaterial(gameObject, material);
  GetMaterial(gameObject);
  SetName(gameObject, string name);
  GetName(gameObject);
  SetBehaviour(gameObject, behaviour);
  GetBehaviour(gameObject, behaviour);

  PARENTING:
  SetParent(parent, child);
  GetParent(child);
  Remove(gameObject);
  GetChildCount(gameObject);
  GetChildAt(gameObject, int index);

  POSITIONING:
  SetLocalPos(position (float x, float y, float z));
  GetWorldPos(gameObject);
  GetLocalPos(gameObject);
  Distance(gameObject1, gameObject2);

  COLLISIONS:
  Collision(string name1, string name2, float collDistance);

  OTHER:
  GetTime();
  KeyPressed(int keyIndex);

  KEY INDEXES:
  enum Key {
    A = 0, B = 1, C = 2, D = 3, E = 4, F = 5,
    G = 6, H = 7, I = 8, J = 9, K = 10, L = 11,
    M = 12, N = 13, O = 14, P = 15, Q = 16, R = 17,
    S = 18, T = 19, U = 20, V = 21, W = 22, X = 23,
    Y = 24, Z = 25,
    Escape = 36, LShift = 38, Space = 57, BackSpace = 59, Tab = 60,
    Left = 71, Right = 72, Up = 73, Down = 74
  }

]]
--The main game script, always loaded, always active.

function Start()

end

function Update()

end
