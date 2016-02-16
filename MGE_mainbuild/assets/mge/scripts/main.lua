--Only called once, at instantiation. Do NOT reference myGameObject here
function Start()
     LoadLevel("out.txt");
     --a = GameObject("ayy lmao",0,10,0);
     --SetMesh(a,LoadMesh("quad.obj"));
     --SetMaterial(a,ColorMaterial(0,1,1));
     --SetMainCamera(world,Camera("cam",0,0,10));
    --print("test");

end

--Called when the script component is attached to a GameObject.
--myGameObject is safe to reference here without having to do nil checks
function OnAttach()
    --print(GetName(myGameObject))
end

--Called every game logic step
function Update()
    --Rotate(a,0.15,0.5,0.5,0);
    if(GetKeyDown(0)) then
    print(RandomInRange(0,9)) end
end

--Collision callback, the argument is the GameObject you are overlapping with
function OnCollision(otherObject)

end
