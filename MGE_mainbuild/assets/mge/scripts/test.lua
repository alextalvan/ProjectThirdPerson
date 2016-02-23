--generic gameobject script


function Start()
    --a = GameObject("ayy lmao",0,0,0);
    --SetMesh(a,LoadMesh("cube_flat.obj"));
    --SetMaterial(a,ColorMaterial(1,0.5,1));
    --SetParent(world,a);
end

function OnAttach()


end

function Update()
print("update")
end

function OnCollision(otherCollider)

end

--active = false;

function OnLevelLoad()
    --g = FindChild(world,"test");
    --Destroy(g);
    --mainScript = FindComponent(world,"LuaScript:main.lua")
    --SetActive(world,true);
    --SetActive(GetOwner(this),false)
    --SetActive(this,true)
    --SetActive(this,false)
    --t = Text("Arial.ttf","test",0,0,0,12,255,255,255)
end

function OnInteract()
    print("interact");
    SetActive(this,false);
    --active = not active;
end
