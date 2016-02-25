--generic gameobject script


function Start()
    --a = GameObject("ayy lmao",0,0,0);
    --SetMesh(a,LoadMesh("cube_flat.obj"));
    --SetMaterial(a,ColorMaterial(1,0.5,1));
    --SetParent(world,a);
    clock = Timer(0);
end

function OnAttach()


end

function Update()
    if(not CheckTimer(clock)) then
      --SetColor(myGameObject,1,0,0);
    else
      --SetColor(myGameObject,1,1,1);
    end
--print("update")
end

function OnCollision(otherCollider)

end

--active = false;

function OnLevelLoad()
    g = FindChild(world,"Raptor(1)");
    SetColor(g,1,1,0);
    --Destroy(g);
    --mainScript = FindComponent(world,"LuaScript:main.lua")
    --SetActive(world,true);
    --SetActive(GetOwner(this),false)
    --SetActive(this,true)
    --SetActive(this,false)
    --t = Text("Arial.ttf","test",0,0,0,12,255,255,255)
end

function OnInteract()


    ResetTimer(clock,5);
    --print("interact");
    --SetActive(this,false);
    --active = not active;
end
