#include "mge/lua/LuaScript.hpp"
#include "mge/core/World.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/materials/ColorMaterial.hpp"
#include "mge/materials/TextureMaterial.hpp"
#include "mge/behaviours/KeysBehaviour.hpp"
#include "mge/behaviours/LookAt.hpp"
#include "mge/config.hpp"
#include "mge/util/Input.hpp"
#include "mge/util/Random.hpp"
#include "mge/core/Time.hpp"
#include "mge/collision/SphereCollider.hpp"
#include "mge/collision/BoxCollider.hpp"
#include "mge/collision/WallCollider.hpp"
#include "mge/editor/LevelImporter.hpp"
#include "mge/gui/GUI.hpp"
#include "mge/gui/GUIText.hpp"
#include "mge/gui/GUISprite.hpp"
#include "mge/gui/GUIButton.hpp"
#include "mge/util/ResourceCacher.hpp"
#include "mge/lua/LuaObject.hpp"
#include "mge/sound/SoundManager.hpp"
#include "mge/util/Timer.hpp"
#include "mge/particles/ParticleSystem.hpp"

//#define MGE_LUA_SAFETY 1  //comment this define out to remove the lua safety checks but heavily improve performance


LuaScript::LuaScript(std::string path, World * world, GUI * world2D)
{

    _name = "LuaScript:" + path;
	//Open lua
	L = lua_open();
	//Open libs
	luaL_openlibs(L);

	//Register functions
	lua_register(L, "LoadMesh", loadMesh);//
	lua_register(L, "LoadTexture", loadTexture);//
	lua_register(L, "ColorMaterial", colorMaterial);//
	lua_register(L, "TextureMaterial", textureMaterial);//
	lua_register(L, "GameObject", gameObject);//
	lua_register(L, "SetName", setName);//
	lua_register(L, "GetName", getName);//
	lua_register(L, "SetMesh", setMesh);//
	lua_register(L, "GetMesh", getMesh);//
	lua_register(L, "SetMaterial", setMaterial);//
	lua_register(L, "GetMaterial", getMaterial);//
	lua_register(L, "Camera", camera);//
	lua_register(L, "AddToWorld", addToWorld);//
	lua_register(L, "SetMainCamera", setMainCamera);//
	lua_register(L, "SetParent", setParent);//
	lua_register(L, "GetParent", getParent);//
	lua_register(L, "GetChildCount", getChildCount);
	lua_register(L, "GetChildAt", getChildAt);
	lua_register(L, "SetLocalPos", setLocalPosition);//
	lua_register(L, "GetWorldPos", getWorldPos);//
	lua_register(L, "SetWorldPos", setWorldPos);//
	lua_register(L, "GetLocalPos", getLocalPos);//
	lua_register(L, "Rotate", rotate);//
	lua_register(L, "Scale", scale);//
	lua_register(L, "Translate", translate);//
	lua_register(L, "GetTime", getTime);//
	lua_register(L, "GetKey", keyPressed);//
	lua_register(L, "GetKeyUp", keyUp);//
	lua_register(L, "GetKeyDown", keyDown);//
	lua_register(L, "GetMouseButton", mouseButton);//
	lua_register(L, "GetMouseButtonUp", mouseButtonUp);//
	lua_register(L, "GetMousePosition", mousePosition);//
	lua_register(L, "AttachComponent", attachComponent);//
	lua_register(L, "Distance", distance);//
	lua_register(L, "RandomInRange", randomRange);//
	lua_register(L, "RollDice", randomRoll);//
	lua_register(L, "SphereCollider", sphereCollider);//
	lua_register(L, "BoxCollider", boxCollider);//
	lua_register(L, "WallCollider", wallCollider);//
	lua_register(L, "LuaScript", luaScript);//
	lua_register(L, "LoadLevel", loadLevel);//
	lua_register(L, "Text", guiText);//
	lua_register(L, "SetTextFont", setTextFont);//
	lua_register(L, "SetTextPosition", setTextPosition);//
	lua_register(L, "SetTextRotation", setTextRotation);//
	lua_register(L, "SetTextString", setTextString);//
	lua_register(L, "SetTextSize", setTextSize);//
	lua_register(L, "SetTextColor", setTextColor);//
	lua_register(L, "Sprite", guiSprite);//
	lua_register(L, "SetSpriteTexture", setSpriteTexture);//
	lua_register(L, "SetSpritePosition", setSpritePosition);//
	lua_register(L, "SetSpriteRotation", setSpriteRotation);//
	lua_register(L, "SetSpriteScale", setSpriteScale);//
	lua_register(L, "Button", guiButton);//
	lua_register(L, "SetButtonTexture", setButtonTexture);//
	lua_register(L, "OnClick", onClick);//
	lua_register(L, "Destroy", destroy);//
	lua_register(L, "GetOwner", getowner);//
	lua_register(L, "FindChild", findChild);//
	lua_register(L, "FindComponent", findComponent);//
	lua_register(L, "CallFunction", luaInvokeFunction);//
	lua_register(L, "CallComplex", luaInvokeFunctionWithArgs);//
	lua_register(L, "PlayMusic", playMusic);//
	lua_register(L, "PlaySFX", playSFX);//
	lua_register(L, "PlaySFX", playSFX);//
	lua_register(L, "SetActive", setActive);//
	lua_register(L, "GetActive", getActive);//
	lua_register(L, "SetColor", setColor);//
	lua_register(L, "Timer", makeTimer);//
	lua_register(L, "ResetTimer", resetTimer);//
	lua_register(L, "CheckTimer", checkTimer);//
	lua_register(L, "SetTextOrigin", setTextOrigin);//
	lua_register(L, "SetSpriteOrigin", setSpriteOrigin);//
	lua_register(L, "SetGUIActive", setGUIActive);//
	lua_register(L, "SetLocalScale", setLocalScale);
	lua_register(L, "GetLocalScale", getLocalScale);
    lua_register(L, "ToggleEmitter", particleToggle);
	lua_register(L, "StartFade", startFade);//
	lua_register(L, "CenterSpriteOrigin", centerSpriteOrigin);//
	lua_register(L, "CenterTextOrigin", centerTextOrigin);//

	//Set world
	lua_pushlightuserdata(L, (LuaObject*)world);
	lua_setglobal(L, "world");

	//Set gui
	lua_pushlightuserdata(L, (LuaObject*)world2D);
	lua_setglobal(L, "GUI");

	//set self reference
	lua_pushlightuserdata(L,((LuaObject*)this));
	lua_setglobal(L, "this");

    //Load file
	luaL_dofile(L, (config::MGE_SCRIPT_PATH + path).c_str());
	//Start
	InvokeFunction("Start");
}

lua_State* LuaScript::getLuaStatePointer()
{
    return L;
}

void LuaScript::InvokeFunction(std::string name)
{
    lua_getglobal(L,name.c_str());
    lua_call(L,0,0);
}

void LuaScript::setOwner(GameObject* pOwner)
{
    Component::setOwner(pOwner);
    if(pOwner==NULL)
    {
        lua_pushnil(L);
        lua_setglobal(L,"myGameObject");
    }
    else
    {
        lua_pushlightuserdata(L, (LuaObject*)pOwner);
        lua_setglobal(L,"myGameObject");
        InvokeFunction("OnAttach");
    }
}

int LuaScript::loadMesh(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isstring(lua, lua_gettop(lua))) throw "Expect: string";
	#endif

	string fileName = lua_tostring(lua, -1);
	//lua_pop(lua,1);


	Mesh* mesh = Mesh::load(config::MGE_MODEL_PATH + fileName);
	lua_pushlightuserdata(lua, mesh);

	return 1;
}

int LuaScript::loadTexture(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isstring(lua, lua_gettop(lua))) throw "Expect: string";
	#endif
	string fileName = lua_tostring(lua, -1);
	//lua_pop(lua,1);

	Texture * texture = Texture::load(config::MGE_TEXTURE_PATH + fileName);
	lua_pushlightuserdata(lua, texture);

	return 1;
}

int LuaScript::colorMaterial(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	glm::vec3 color = glm::vec3(0, 0, 0);
	color.x = lua_tonumber(lua, -3);
	color.y = lua_tonumber(lua, -2);
	color.z = lua_tonumber(lua, -1);
	//lua_pop(lua,3);

	AbstractMaterial* newMaterial = new ColorMaterial(color);
	lua_pushlightuserdata(lua, newMaterial);

	return 1;
}

int LuaScript::setColor(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

    GameObject* obj = (GameObject*)(LuaObject*)lua_touserdata(lua,-4);
	glm::vec3 color = glm::vec3(0, 0, 0);
	color.x = lua_tonumber(lua, -3);
	color.y = lua_tonumber(lua, -2);
	color.z = lua_tonumber(lua, -1);
	//lua_pop(lua,3);

	//std::string s = obj->getName();

	obj->getMaterial()->color = color;

	return 0;
}

int LuaScript::textureMaterial(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: texture";
	#endif

	Texture * texture = (Texture*)lua_touserdata(lua, -1);
	//lua_pop(lua,1);

	AbstractMaterial* newMaterial = new TextureMaterial(texture);
	lua_pushlightuserdata(lua, newMaterial);

	return 1;
}

int LuaScript::gameObject(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isstring(lua, -4)) throw "Expect: string";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	string name = lua_tostring(lua, -4);
	glm::vec3 position = glm::vec3(0, 0, 0);
	position.x = lua_tonumber(lua, -3);
	position.y = lua_tonumber(lua, -2);
	position.z = lua_tonumber(lua, -1);

	GameObject * gameObj = new GameObject(name, position);

	lua_getglobal(lua,"world");
	World* w = (World*)(LuaObject*)lua_touserdata(lua,-1);
	w->AddChild(gameObj);

    //lua_pop(lua,5);
	lua_pushlightuserdata(lua,(LuaObject*)gameObj);

	return 1;
}

int LuaScript::guiSprite(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isstring(lua, -6)) throw "Expect: string";
	if (!lua_isnumber(lua, -5)) throw "Expect: number";
	if (!lua_isnumber(lua, -4)) throw "Expect: number";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	glm::vec2 position = glm::vec2(0, 0);
	glm::vec2 scale = glm::vec2(0, 0);

	string textureName = lua_tostring(lua, -6);
	position.x = lua_tonumber(lua, -5);
	position.y = lua_tonumber(lua, -4);
	float rotation = lua_tonumber(lua, -3);
	scale.x = lua_tonumber(lua, -2);
	scale.y = lua_tonumber(lua, -1);

	lua_getglobal(lua,"GUI");
	GUI * gui = (GUI*)(LuaObject*)lua_touserdata(lua,-1);
	GUISprite * newGuiSprite = new GUISprite(*Utils::LoadTexture(textureName), position.x , position.y ,rotation, scale.x, scale.y);
	gui->AddChild(newGuiSprite);
	lua_pushlightuserdata(lua,(LuaObject*) newGuiSprite);

	return 1;
}

int LuaScript::guiText(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isstring(lua, -10)) throw "Expect: string";
	if (!lua_isstring(lua, -9)) throw "Expect: string";
	if (!lua_isnumber(lua, -8)) throw "Expect: number";
	if (!lua_isnumber(lua, -7)) throw "Expect: number";
	if (!lua_isnumber(lua, -6)) throw "Expect: number";
	if (!lua_isnumber(lua, -5)) throw "Expect: number";
	if (!lua_isnumber(lua, -4)) throw "Expect: number";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	glm::vec2 position = glm::vec2(0, 0);

	string fontName = lua_tostring(lua, -10);
	string text = lua_tostring(lua, -9);
	position.x = lua_tonumber(lua, -8);
	position.y = lua_tonumber(lua, -7);
	float rotation = lua_tonumber(lua, -6);
	int textSize = lua_tonumber(lua, -5);
	int r = lua_tonumber(lua, -4);
	int g = lua_tonumber(lua, -3);
	int b = lua_tonumber(lua, -2);
	int a = lua_tonumber(lua, -1);

	lua_getglobal(lua,"GUI");
	GUI * gui = (GUI*)(LuaObject*)lua_touserdata(lua,-1);
	GUIText * newGuiText = new GUIText(*Utils::LoadFont(fontName), text, position.x, position.y, rotation, textSize, sf::Color(r, g, b, a));

	gui->AddChild(newGuiText);
	lua_pushlightuserdata(lua,(LuaObject*) newGuiText);

	return 1;
}

int LuaScript::guiButton(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isstring(lua, -7)) throw "Expect: string";
	if (!lua_isstring(lua, -6)) throw "Expect: string";
	if (!lua_isnumber(lua, -5)) throw "Expect: number";
	if (!lua_isnumber(lua, -4)) throw "Expect: number";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	glm::vec2 position = glm::vec2(0, 0);
	glm::vec2 scale = glm::vec2(0, 0);

	string texture1Name = lua_tostring(lua, -7);
	string texture2Name = lua_tostring(lua, -6);
	position.x = lua_tonumber(lua, -5);
	position.y = lua_tonumber(lua, -4);
	scale.x = lua_tonumber(lua, -3);
	scale.y = lua_tonumber(lua, -2);
	float rotation = lua_tonumber(lua, -1);

	lua_getglobal(lua,"GUI");
	GUI * gui = (GUI*)(LuaObject*)lua_touserdata(lua, -1);
	GUIButton * newGuiButton = new GUIButton(*Utils::LoadTexture(texture1Name), *Utils::LoadTexture(texture2Name), position.x, position.y, scale.x, scale.y, rotation);

	gui->AddChild(newGuiButton);
	lua_pushlightuserdata(lua,(LuaObject*) newGuiButton);

	return 1;
}

int LuaScript::setButtonTexture(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -3)) throw "Expect: game object";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GUIButton * guiButton = (GUIButton*)(LuaObject*)lua_touserdata(lua, -3);
	string textureName = lua_tostring(lua, -2);
	int index = lua_tonumber(lua, -1);

	guiButton->setSpriteTexture(*Utils::LoadTexture(textureName), index);

	return 0;
}

int LuaScript::setTextFont(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isstring(lua, -1)) throw "Expect: string";
	#endif

	GUIText * guiText = (GUIText*)(LuaObject*)lua_touserdata(lua, -2);
	string fontName = lua_tostring(lua, -1);
	guiText->setTextFont(*Utils::LoadFont(fontName));

	return 0;
}

int LuaScript::setTextPosition(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -3)) throw "Expect: game object";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GUIText * guiText = (GUIText*)(LuaObject*)lua_touserdata(lua, -3);
	float x = lua_tonumber(lua, -2);
	float y = lua_tonumber(lua, -1);
	guiText->setTextPosition(x, y);

	return 0;
}

int LuaScript::setTextRotation(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GUIText * guiText = (GUIText*)(LuaObject*)lua_touserdata(lua, -2);
	float rot = lua_tonumber(lua, -1);
	guiText->setTextRotation(rot);

	return 0;
}

int LuaScript::setTextSize(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GUIText * guiText = (GUIText*)(LuaObject*)lua_touserdata(lua, -2);
	int size = lua_tonumber(lua, -1);
	guiText->setTextSize(size);

	return 0;
}

int LuaScript::setTextString(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isstring(lua, -1)) throw "Expect: string";
	#endif

	GUIText * guiText = (GUIText*)(LuaObject*)lua_touserdata(lua, -2);
	string text = lua_tostring(lua, -1);
	guiText->setTextString(text);

	return 0;
}

int LuaScript::setTextColor(lua_State * lua)
{
     #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -5)) throw "Expect: game object";
	if (!lua_isnumber(lua, -4)) throw "Expect: number";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GUIText * guiText = (GUIText*)(LuaObject*)lua_touserdata(lua,-5);
	int r = lua_tonumber(lua, -4);
	int g = lua_tonumber(lua, -3);
	int b = lua_tonumber(lua, -2);
	int a = lua_tonumber(lua, -1);
	guiText->setTextColor(sf::Color(r, g, b, a));

	return 0;
}

int LuaScript::setSpriteTexture(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isstring(lua, -1)) throw "Expect: string";
	#endif

	GUISprite * guiSprite = (GUISprite*)(LuaObject*)lua_touserdata(lua, -2);
	string textureName = lua_tostring(lua, -1);
	guiSprite->setSpriteTexture(*Utils::LoadTexture(textureName));

	return 0;
}

int LuaScript::setSpritePosition(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -3)) throw "Expect: game object";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GUISprite * guiSprite = (GUISprite*)(LuaObject*)lua_touserdata(lua, -3);
	float x = lua_tonumber(lua, -2);
	float y = lua_tonumber(lua, -1);
	guiSprite->setSpritePosition(x, y);

	return 0;
}

int LuaScript::setSpriteScale(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -3)) throw "Expect: game object";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GUISprite * guiSprite = (GUISprite*)(LuaObject*)lua_touserdata(lua, -3);
	float x = lua_tonumber(lua, -2);
	float y = lua_tonumber(lua, -1);
	guiSprite->setSpriteScale(x, y);

	return 0;
}

int LuaScript::setSpriteRotation(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GUISprite * guiSprite = (GUISprite*)(LuaObject*)lua_touserdata(lua, -2);
	float rot = lua_tonumber(lua, -1);
	guiSprite->setSpriteRotation(rot);

	return 0;
}

int LuaScript::onClick(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object";
	#endif

	GUIButton * guiButton = (GUIButton*)(LuaObject*)lua_touserdata(lua, -1);
	bool clicked = false;
	if (guiButton->onClick()) {
        clicked = true;
	}

    lua_pushboolean(lua, clicked);

	return 1;
}

int LuaScript::setSpriteOrigin(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -3)) throw "Expect: game object";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GUISprite * guiSprite = (GUISprite*)(LuaObject*)lua_touserdata(lua, -3);
	float x = lua_tonumber(lua, -2);
	float y = lua_tonumber(lua, -1);
	guiSprite->setOrigin(x, y);

	return 0;
}

int LuaScript::setTextOrigin(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -3)) throw "Expect: game object";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GUIText * guiText = (GUIText*)(LuaObject*)lua_touserdata(lua, -3);
	float x = lua_tonumber(lua, -2);
	float y = lua_tonumber(lua, -1);
	guiText->setOrigin(x, y);

	return 0;
}

int LuaScript::centerSpriteOrigin(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object";
	#endif

	GUISprite * guiSprite = (GUISprite*)(LuaObject*)lua_touserdata(lua, -1);
	guiSprite->setOrigin();

	return 0;
}

int LuaScript::centerTextOrigin(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object";
	#endif

	GUIText * guiText = (GUIText*)(LuaObject*)lua_touserdata(lua, -1);
	guiText->setOrigin();

	return 0;
}

int LuaScript::setGUIActive(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isboolean(lua, -1)) throw "Expect: boolean";
	#endif

	GUI * gui = (GUI*)(LuaObject*)lua_touserdata(lua, -2);
	bool active = lua_toboolean(lua, -1);
	gui->SetGUIActive(active);

	return 0;
}

int LuaScript::startFade(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -3)) throw "Expect: game object";
	if (!lua_isboolean(lua, -2)) throw "Expect: boolean";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GUI * gui = (GUI*)(LuaObject*)lua_touserdata(lua, -3);
	bool state = lua_toboolean(lua, -2);
	int speed = lua_tonumber(lua, -1);
	gui->startFade(state, speed);

	return 0;
}

int LuaScript::camera(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isstring(lua, -4)) throw "Expect: string";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	string name = lua_tostring(lua, -4);
	glm::vec3 position = glm::vec3(0, 0, 0);
	position.x = lua_tonumber(lua, -3);
	position.y = lua_tonumber(lua, -2);
	position.z = lua_tonumber(lua, -1);

	Camera * cam = new Camera(name, position);

	lua_getglobal(lua,"world");
	World* w = (World*)(LuaObject*) lua_touserdata(lua,-1);
	w->AddChild(cam);
	lua_pushlightuserdata(lua, (LuaObject*)cam);

	return 1;
}

int LuaScript::addToWorld(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: world";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object";
	#endif

	World * world = (World*)(LuaObject*)lua_touserdata(lua, -2);
	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -1);

	world->AddChild(gameObj);

	return 0;
}

int LuaScript::setMainCamera(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: world";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: camera";
	#endif

	World * world = (World*)(LuaObject*)lua_touserdata(lua, -2);
	Camera * cam = (Camera*)(LuaObject*)lua_touserdata(lua, -1);

	world->setMainCamera(cam);

	return 0;
}

int LuaScript::setParent(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object";
	#endif

	GameObject * parent = (GameObject*)(LuaObject*)lua_touserdata(lua, -2);
	GameObject * child = (GameObject*)(LuaObject*)lua_touserdata(lua, -1);

	parent->AddChild(child);

	//2d
	GUI* guiCast = dynamic_cast<GUI*>(child);
	if(guiCast!=NULL)
    {
        guiCast->Refresh2DTransform();
    }

	return 0;
}

int LuaScript::getParent(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObject = (GameObject*)(LuaObject*)lua_touserdata(lua, -1);
	GameObject * parent = gameObject->getParent();

	lua_pushlightuserdata(lua, (LuaObject*)parent);

	return 1;
}

int LuaScript::getowner(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: component";
	#endif

	Component * comp = (Component*)(LuaObject*)lua_touserdata(lua, -1);

    GameObject* owner = comp->getOwner();

    if(owner==NULL)
        lua_pushnil(lua);
    else
        lua_pushlightuserdata(lua, (LuaObject*)owner);

	return 1;
}

int LuaScript::getChildCount(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObject = (GameObject*)(LuaObject*)lua_touserdata(lua, -1);
	int count = gameObject->GetChildCount();

	lua_pushnumber(lua, count);

	return 1;
}

int LuaScript::getChildAt(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GameObject * gameObject = (GameObject*)(LuaObject*)lua_touserdata(lua, -2);
	int index = lua_tonumber(lua, -2);
	GameObject * child = gameObject->GetChildAt(index);

	lua_pushlightuserdata(lua, child);

	return 1;
}

int LuaScript::setLocalPosition(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -4)) throw "Expect: game object";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -4);
	glm::vec3 position = glm::vec3(0, 0, 0);
	position.x = lua_tonumber(lua, -3);
	position.y = lua_tonumber(lua, -2);
	position.z = lua_tonumber(lua, -1);

	gameObj->setLocalPosition(position);

	return 0;
}

int LuaScript::getWorldPos(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -1);

	glm::vec3 pos = gameObj->getWorldPosition();

	lua_pushnumber(lua, pos.x);
	lua_pushnumber(lua, pos.y);
	lua_pushnumber(lua, pos.z);

	return 3;
}

int LuaScript::setWorldPos(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -4);
	glm::vec3 position = glm::vec3(0, 0, 0);
	position.x = lua_tonumber(lua, -3);
	position.y = lua_tonumber(lua, -2);
	position.z = lua_tonumber(lua, -1);

	gameObj->setWorldPosition(position);

	return 0;
}

int LuaScript::getLocalPos(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -1);

	glm::vec3 pos = gameObj->getLocalPosition();

	lua_pushnumber(lua, pos.x);
	lua_pushnumber(lua, pos.y);
	lua_pushnumber(lua, pos.z);

	return 3;
}

int LuaScript::rotate(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -5)) throw "Expect: game object";
	if (!lua_isnumber(lua, -4)) throw "Expect: number";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -5);
	float angle = lua_tonumber(lua, -4);
	glm::vec3 axis = glm::vec3(0, 0, 0);
	axis.x = lua_tonumber(lua, -3);
	axis.y = lua_tonumber(lua, -2);
	axis.z = lua_tonumber(lua, -1);

	gameObj->rotate(glm::radians(angle), axis);

	return 0;
}

int LuaScript::scale(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -4)) throw "Expect: game object";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -4);
	glm::vec3 scalar = glm::vec3(0, 0, 0);
	scalar.x = lua_tonumber(lua, -3);
	scalar.y = lua_tonumber(lua, -2);
	scalar.z = lua_tonumber(lua, -1);

	gameObj->scale(scalar);

	return 0;
}

int LuaScript::translate(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -4)) throw "Expect: game object";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif

	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -4);
	glm::vec3 translation = glm::vec3(0, 0, 0);
	translation.x = lua_tonumber(lua, -3);
	translation.y = lua_tonumber(lua, -2);
	translation.z = lua_tonumber(lua, -1);

	gameObj->translate(translation);

	return 0;
}

int LuaScript::setMesh(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: mesh";
	#endif

	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -2);
	Mesh * mesh = (Mesh*)lua_touserdata(lua, -1);

	gameObj->setMesh(mesh);

	return 0;
}

int LuaScript::getMesh(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -1);
	Mesh * mesh = gameObj->getMesh();

	lua_pushlightuserdata(lua, mesh);

	return 1;
}

int LuaScript::setMaterial(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: material";
	#endif

	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -2);
	AbstractMaterial * material = (AbstractMaterial*)lua_touserdata(lua, -1);

	gameObj->setMaterial(material);

	return 0;
}

int LuaScript::getMaterial(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -1);
	AbstractMaterial * material = gameObj->getMaterial();

	lua_pushlightuserdata(lua, material);

	return 1;
}

int LuaScript::setName(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isstring(lua, -1)) throw "Expect: string";
	#endif

	string name = lua_tostring(lua, -1);
	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -2);

	gameObj->setName(name);

	return 0;
}

int LuaScript::getName(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, lua_gettop(lua))) throw "Expect: game object";
	#endif

	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -1);
	const char * name = gameObj->getName().c_str();

	lua_pushstring(lua, name);

	return 1;
}

int LuaScript::getTime(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	#endif
	float time = Time::now();
	lua_pushnumber(lua, time);

	return 1;
}

int LuaScript::keyPressed(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isnumber(lua, lua_gettop(lua))) throw "Expect: number";
	#endif

	int keyIndex = lua_tonumber(lua, -1);
	lua_pushboolean(lua, Input::GetKey((Input::KeyCode)keyIndex));

	return 1;
}

int LuaScript::keyUp(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isnumber(lua, lua_gettop(lua))) throw "Expect: number";
	#endif

	int keyIndex = lua_tonumber(lua, -1);
	lua_pushboolean(lua, Input::GetKeyUp((Input::KeyCode)keyIndex));

	return 1;
}

int LuaScript::keyDown(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isnumber(lua, lua_gettop(lua))) throw "Expect: number";
	#endif

	int keyIndex = lua_tonumber(lua, -1);
	lua_pushboolean(lua, Input::GetKeyDown((Input::KeyCode)keyIndex));

	return 1;
}

int LuaScript::mouseButton(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isnumber(lua, lua_gettop(lua))) throw "Expect: number";
	#endif

	int button = lua_tonumber(lua, -1);
	lua_pushboolean(lua, Input::GetMouseButton(button));

	return 1;
}

int LuaScript::mouseButtonUp(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isnumber(lua, lua_gettop(lua))) throw "Expect: number";
	#endif

	int button = lua_tonumber(lua, -1);
	lua_pushboolean(lua, Input::GetMouseButtonUp(button));

	return 1;
}

int LuaScript::mousePosition(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	#endif
    lua_pushinteger(lua,Input::getMouseX());
    lua_pushinteger(lua,Input::getMouseY());
	return 2;
}

int LuaScript::randomRange(lua_State * lua)
{
    int r1 = lua_tonumber(lua, -2);
    int r2 = lua_tonumber(lua, -1);

    //lua_pop(lua,2);

    lua_pushinteger(lua,Utils::Random::GetValue(r1,r2));
	return 1;
}


int LuaScript::randomRoll(lua_State * lua)
{
    int sides = lua_tonumber(lua, -1);
    //lua_pop(lua,1);
    lua_pushboolean(lua,Utils::Random::Roll(sides));
	return 1;
}



int LuaScript::attachComponent(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY

	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: behaviour";
	#endif

	GameObject * gameObj = (GameObject*)(LuaObject*)lua_touserdata(lua, -2);
	Component * comp = (Component*)(LuaObject*)lua_touserdata(lua, -1);
	//lua_pop(lua,2);

	gameObj->AttachComponent(comp);

	return 0;

}

int LuaScript::sphereCollider(lua_State * lua)
{
    SphereCollider* sphere = new SphereCollider();
    float rad = lua_tonumber(lua, -1);
    //lua_pop(lua,1);
    sphere->radius = rad;
    lua_pushlightuserdata(lua,(LuaObject*)sphere);
    return 1;
}

int LuaScript::boxCollider(lua_State * lua)
{
    BoxCollider* box = new BoxCollider();
    float x = lua_tonumber(lua, -3);
    float y = lua_tonumber(lua, -2);
    float z = lua_tonumber(lua, -1);
    //lua_pop(lua,3);

    box->xSize = x;
    box->ySize = y;
    box->zSize = z;
    lua_pushlightuserdata(lua,(LuaObject*)box);
    return 1;
}

int LuaScript::wallCollider(lua_State * lua)
{
    WallCollider* wall = new WallCollider();
    float x = lua_tonumber(lua, -3);
    float y = lua_tonumber(lua, -2);
    float z = lua_tonumber(lua, -1);
    //lua_pop(lua,3);

    wall->xSize = x;
    wall->ySize = y;
    wall->zSize = z;
    lua_pushlightuserdata(lua,(LuaObject*)wall);
    return 1;
}

int LuaScript::luaScript(lua_State * lua)
{
    string fileName = lua_tostring(lua, -1);
    lua_getglobal(lua,"world");
    World* world = (World*)(LuaObject*)lua_touserdata(lua,-1);
    lua_getglobal(lua,"GUI");
    GUI * gui = (GUI*)(LuaObject*)lua_touserdata(lua,-1);

    //lua_pop(lua,3);

    LuaScript* script = new LuaScript(fileName,world, gui);
    lua_pushlightuserdata(lua,(LuaObject*)script);
    return 1;
}

int LuaScript::loadLevel(lua_State * lua)
{
    string fileName = lua_tostring(lua, -1);

    lua_getglobal(lua,"world");
    World* world = (World*)(LuaObject*)lua_touserdata(lua,-1);

    lua_getglobal(lua,"GUI");
    GUI * gui = (GUI*)(LuaObject*)lua_touserdata(lua,-1);

    //lua_pop(lua,3);

    LevelEditor::LoadLevel(config::MGE_LEVEL_PATH + fileName,world,gui);
    return 0;
}


int LuaScript::distance(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object";
	#endif

	GameObject * gameObj1 = (GameObject*)(LuaObject*)lua_touserdata(lua, -2);
	GameObject * gameObj2 = (GameObject*)(LuaObject*)lua_touserdata(lua, -1);
	//lua_pop(lua,2);

	glm::vec3 pos1 = gameObj1->getWorldPosition();
	glm::vec3 pos2 = gameObj2->getWorldPosition();
	float distance = glm::distance(pos1, pos2);

	lua_pushnumber(lua, distance);

	return 1;
}

void LuaScript::Update()
{
	lua_getglobal(L, "Update");
	lua_call(L, 0, 0);
	//cout<<_name<<" stack size: "<<lua_gettop(L)<<"\n";
}

void LuaScript::InvokeCollisionCallback(GameObject* other)
{
    lua_getglobal(L, "OnCollision");
    lua_pushlightuserdata(L,(LuaObject*)other);
	lua_call(L, 1, 0);
}

int LuaScript::destroy(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object or component";
	#endif


    LuaObject* l = (LuaObject*)lua_touserdata(lua,-1);

    GameObject* g = dynamic_cast<GameObject*>(l);
    if(g!=NULL){g->Destroy(); return 0;}

    Component* c = dynamic_cast<Component*>(l);
    if(c!=NULL) {c->Destroy(); return 0;}

	return 0;
}

int LuaScript::findChild(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
    if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isstring(lua, -1)) throw "Expect: string";
	#endif


	GameObject* obj = (GameObject*)(LuaObject*)lua_touserdata(lua, -2);
	string name = lua_tostring(lua,-1);

    GameObject* child = obj->FindChild(name);
	if(child==NULL)
        lua_pushnil(lua);
    else
        lua_pushlightuserdata(lua,(LuaObject*)child);

	return 1;
}

int LuaScript::findComponent(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
    if (!lua_islightuserdata(lua, -2)) throw "Expect: game object";
	if (!lua_isstring(lua, -1)) throw "Expect: string";
	#endif

	GameObject* obj = (GameObject*)(LuaObject*)lua_touserdata(lua, -2);
	string name = lua_tostring(lua,-1);

    Component* comp = obj->FindComponent(name);
	if(comp==NULL)
        lua_pushnil(lua);
    else
        lua_pushlightuserdata(lua,(LuaObject*)comp);

	return 1;
}

int LuaScript::playMusic(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_isstring(lua, -3)) throw "Expect: string";
	#endif

	bool loop = lua_toboolean(lua,-1);
	float vol = lua_tonumber(lua,-2);
	string name = lua_tostring(lua,-3);

    Sound::PlayMusic((config::MGE_SOUND_PATH + name).c_str(),vol,loop);

	return 0;
}

int LuaScript::playSFX(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	#endif

	float vol = lua_tonumber(lua,-1);
	string name = lua_tostring(lua,-2);

    Sound::PlaySFX((config::MGE_SOUND_PATH + name).c_str(),vol);

	return 0;
}

int LuaScript::setActive(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object or component";
	#endif


    LuaObject* l = (LuaObject*)lua_touserdata(lua,-2);
    bool val = lua_toboolean(lua,-1);

    GameObject* g = dynamic_cast<GameObject*>(l);
    if(g!=NULL){g->SetActive(val); return 0;}

    Component* c = dynamic_cast<Component*>(l);
    if(c!=NULL) {c->SetActive(val); return 0;}

    return 0;
}

int LuaScript::getActive(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -1)) throw "Expect: game object or component";
	#endif

    LuaObject* l = (LuaObject*)lua_touserdata(lua,-1);

    GameObject* g = dynamic_cast<GameObject*>(l);
    if(g!=NULL){lua_pushboolean(lua,g->IsActive()); return 1;}

    Component* c = dynamic_cast<Component*>(l);
    if(c!=NULL){lua_pushboolean(lua,c->IsActive()); return 1;}

    return 0;
}

int LuaScript::luaInvokeFunction(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
    if (!lua_islightuserdata(lua, -2)) throw "Expect: component";
	if (!lua_isstring(lua, -1)) throw "Expect: string";
	#endif

	LuaScript* comp = (LuaScript*)(LuaObject*)lua_touserdata(lua, -2);
	string name = lua_tostring(lua,-1);

    comp->InvokeFunction(name);

	return 0;
}

int LuaScript::luaInvokeFunctionWithArgs(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
    if (!lua_islightuserdata(lua, -2)) throw "Expect: component";
	if (!lua_isstring(lua, -1)) throw "Expect: string";
	#endif

	//first take and pop the number of args and results passed by this function
	int nArg = lua_tonumber(lua,-2);
	int nRes = lua_tonumber(lua,-1);
	//lua_pop(lua,2);

	LuaScript* script = (LuaScript*)(LuaObject*)lua_touserdata(lua,-nArg-4);
	std::string func = lua_tostring(lua,-nArg-3);

	lua_State* otherL = script->getLuaStatePointer();

	float n; std::string s; int b; void* v;


	//now pushing the arguments on the target script's stack
	lua_getglobal(otherL,func.c_str());

	for(int i = -(nArg-1)-3; i<-2; ++i)
    {
        int argType = lua_type(lua,i);

        switch(argType)
        {
        case LUA_TNUMBER:
            n = lua_tonumber(lua,i);
            lua_pushnumber(otherL,n);
            break;
        case LUA_TSTRING:
            s = lua_tostring(lua,i);
            lua_pushstring(otherL,s.c_str());
            break;
        case LUA_TBOOLEAN:
            b = lua_toboolean(lua,i);
            lua_pushboolean(otherL,b);
            break;
        case LUA_TLIGHTUSERDATA:
            v = lua_touserdata(lua,i);
            lua_pushlightuserdata(otherL,v);
            break;
        }
    }

    //the actual function invoke

    lua_call(otherL,nArg,nRes);

    //now popping the results and putting them in the first script

    for(int i = -nRes; i<0; ++i)
    {
        int argType = lua_type(otherL,i);

        switch(argType)
        {
        case LUA_TNUMBER:
            n = lua_tonumber(otherL,i);
            lua_pushnumber(lua,n);
            break;
        case LUA_TSTRING:
            s = lua_tostring(otherL,i);
            lua_pushstring(lua,s.c_str());
            break;
        case LUA_TBOOLEAN:
            b = lua_toboolean(otherL,i);
            lua_pushboolean(lua,b);
            break;
        case LUA_TLIGHTUSERDATA:
            v = lua_touserdata(otherL,i);
            lua_pushlightuserdata(lua,v);
            break;
        }
    }

    lua_pop(otherL,nRes);//without this it leaks memory

	return nRes;
}

int LuaScript::setLocalScale(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -4)) throw "Expect: game object";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif


	GameObject * obj = (GameObject*)(LuaObject*)lua_touserdata(lua, -4);
	float x = lua_tonumber(lua, -3);
	float y = lua_tonumber(lua, -2);
	float z = lua_tonumber(lua, -1);

    obj->setLocalScale(glm::vec3(x,y,z));
	//obj->

	return 0;
}

int LuaScript::getLocalScale(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	if (!lua_islightuserdata(lua, -4)) throw "Expect: game object";
	if (!lua_isnumber(lua, -3)) throw "Expect: number";
	if (!lua_isnumber(lua, -2)) throw "Expect: number";
	if (!lua_isnumber(lua, -1)) throw "Expect: number";
	#endif


	GameObject * obj = (GameObject*)(LuaObject*)lua_touserdata(lua, -1);

    glm::vec3 scale = obj->getLocalScale();

    lua_pushnumber(lua,scale.x);
    lua_pushnumber(lua,scale.y);
    lua_pushnumber(lua,scale.z);
	//obj->

	return 3;
}

int LuaScript::makeTimer(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	#endif

    float dur = lua_tonumber(lua,-1);
    Timer* t = new Timer(dur);
    t->Reset();

	lua_pushlightuserdata(lua, t);

	return 1;
}

int LuaScript::resetTimer(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	#endif

    Timer* t = (Timer*)lua_touserdata(lua,-2);
    float dur = lua_tonumber(lua,-1);
    t->SetDuration(dur);
    t->Reset();

	return 0;
}

int LuaScript::checkTimer(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	#endif

    Timer* t = (Timer*)lua_touserdata(lua,-1);

    lua_pushboolean(lua,t->isFinished());

	return 1;
}

int LuaScript::particleToggle(lua_State * lua)
{
    #ifdef MGE_LUA_SAFETY
	#endif

    //Timer* t = (Timer*)lua_touserdata(lua,-2);
    ParticleSystem* p = (ParticleSystem*)(GameObject*)(LuaObject*)lua_touserdata(lua,-2);
    bool val = lua_toboolean(lua,-1);

    p->ToggleEmitter(val);

	return 0;
}

LuaScript::~LuaScript()
{
	lua_close(L);
}
