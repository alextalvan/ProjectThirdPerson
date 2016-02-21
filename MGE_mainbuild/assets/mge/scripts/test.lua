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

end

function OnCollision(otherCollider)
	if(GetName(otherCollider)~= "floor") then
        Destroy(GetOwner(this)) end
end

function OnLevelLoad()
    --g = FindChild(world,"test");
    --Destroy(g);
    --mainScript = FindComponent(world,"LuaScript:main.lua")
end
