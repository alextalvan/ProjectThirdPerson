--Only called once, at instantiation. Do NOT reference myGameObject here
function Start()

end

--Called when the script component is attached to a GameObject.
--myGameObject is safe to reference here without having to do nil checks
function OnAttach()

end

--Called every game logic step
function Update()
x,y,z = GetWorldPos(player)
	RotateTo(myGameObject, x,y,z, 0.25)
end

--Collision callback, the argument is the GameObject you are overlapping with
function OnCollision(otherObject)

end

--Called after a level loads, all gameobjects are loaded and safe to find/reference here
function OnLevelLoad()
player = FindChild(world,"Character")

end

--Called when the player interacts with this object via raycast on the collider
function OnInteract()

end

function Lerp(x1,y1,z1,x2,y2,z2,t)

 x3 = x1 + (x2 -x1) * t;
 y3 = y1 + (y2 -y1) * t;
 z3 = z1 + (z2 -z1) * t;

 return x3,y3,z3;
end
