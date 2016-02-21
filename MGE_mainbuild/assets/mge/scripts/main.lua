--Only called once, at instantiation. Do NOT reference myGameObject here
function Start()


     --b = Sprite("Osama.jpg",500,500,45,1,1);
     --SetParent(a,b);
     --SetSpritePosition(b,100,100);
     --a = GameObject("ayy lmao",0,10,0);
     --SetMesh(a,LoadMesh("quad.obj"));
     --SetMaterial(a,ColorMaterial(0,1,1));
     --SetMainCamera(world,Camera("cam",0,0,10));
    --print("test");
    --t = ColorMaterial(1,0,1);

end

--Called when the script component is attached to a GameObject.
--myGameObject is safe to reference here without having to do nil checks
function OnAttach()
    --print(GetName(myGameObject))
    LoadLevel("out.txt");
    --a = GameObject("topkek",123,124,125);
    a = Sprite("bricks.jpg",683,384,0,0.1,0.1);
end

--Called every game logic step
function Update()
    --SetSpriteScale(a, 2, 2);
    --Rotate(a,0.15,0.5,0.5,0);
    --if(GetKeyDown(0)) then
     --   Destroy(b); end

end

--Collision callback, the argument is the GameObject you are overlapping with
function OnCollision(otherObject)

end

function TestFunction(a,b,c)
    print(a)
    print(b)
    print(c)
end

function TestFunction2()
    print("test2")
end

function TestFunction3()
    return 5,"lmfao",a,90,false;
end

function TestFunction4(g,h)
    print(g)
    print(h)
    x,y,z = GetWorldPos(a);
    print(x) print(y) print(z)
    return "top","kek",0;
end

function OnLevelLoad()
    --TestFunction();
end
