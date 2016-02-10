--generic gameobject script


function Start()

end

function Update()
    if(GetKeyUp(0)) then
    print(RandomInRange(0,10)); end
end

function OnCollision(otherCollider)
	--print(GetName(otherCollider));
end
