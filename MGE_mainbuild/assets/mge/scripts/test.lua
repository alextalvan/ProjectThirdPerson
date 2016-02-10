--generic gameobject script


function Start()
    a = GameObject("ayy lmao",0,0,0);
    SetMesh(a,LoadMesh("cube_flat.obj"));
    SetMaterial(a,ColorMaterial(1,0.5,1));
    SetParent(world,a);
    s = LuaScript("template.lua");
    AttachComponent(a,s);
end

function OnAttach()

end

function Update()
    --if(GetKeyUp(0)) then
    --print(RandomInRange(0,10)); end

end

function OnCollision(otherCollider)
	--print(GetName(otherCollider));
end
