--Only called once, at instantiation. Do NOT reference myGameObject here
function Start()

end

--Called when the script component is attached to a GameObject.
--myGameObject is safe to reference here without having to do nil checks
function OnAttach()

end

--Called every game logic step
function Update()
    print(GetName(myGameObject))
end

--Collision callback, the argument is the GameObject you are overlapping with
function OnCollision(otherObject)

end
