--Only called once, at instantiation. Do NOT reference myGameObject here
function Start()
    g = GameObject("test",0,0,0);
    AttachComponent(g,LuaScript("test.lua"))
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
