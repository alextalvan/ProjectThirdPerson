--generic gameobject script


function Start()

end

function Update()
    if(GetKeyDown(0)) then
        print("test")
    end
end

function OnCollision(otherCollider)
	--print(GetName(otherCollider));
end
