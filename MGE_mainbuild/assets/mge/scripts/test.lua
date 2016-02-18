--generic gameobject script


function Start()
   -- a = GameObject("ayy lmao",0,0,0);
    --SetMesh(a,LoadMesh("cube_flat.obj"));
    --SetMaterial(a,ColorMaterial(1,0.5,1));
    --SetParent(world,a);
end

function OnAttach()

end

function Update()
    --if(GetKeyUp(0)) then
    --print(RandomInRange(0,10)); end
    --("");

    --print(FindChild(world,"Character"))
end

function OnCollision(otherCollider)
	--
	if(GetName(otherCollider)~="floor") then
        print(GetName(otherCollider));
        Destroy(GetOwner(this)); end
end
