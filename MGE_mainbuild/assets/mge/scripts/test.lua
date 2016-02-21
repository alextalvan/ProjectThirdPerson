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
    if(GetKeyUp(57)) then

        --print(mainScript)
        --CallComplex(mainScript,"TestFunction",10,"testString","ayy",3,0) end
        x,y,z = CallComplex(mainScript,"TestFunction4",50,700,2,3);
        print(x)
        print(y)
        print(z)
        end

end

function OnCollision(otherCollider)
	--
end

function OnLevelLoad()
    --g = FindChild(world,"test");
    --Destroy(g);
    mainScript = FindComponent(world,"LuaScript:main.lua")
end
