--Only called once, at instantiation. Do NOT reference myGameObject here
function Start()
    --a = GameObject("ayy lmao",0,0,5);
    --SetMesh(a,LoadMesh("cube_flat.obj"));
    --SetMaterial(a,ColorMaterial(0,1,1));
    --SetMainCamera(world,Camera("cam",0,0,10));
    --print("test");
    LoadLevel("out.txt");
end

--Called when the script component is attached to a GameObject.
--myGameObject is safe to reference here without having to do nil checks
function OnAttach()
    --print(GetName(myGameObject))
end

--Called every game logic step
function Update()

end

--Collision callback, the argument is the GameObject you are overlapping with
function OnCollision(otherObject)

end
