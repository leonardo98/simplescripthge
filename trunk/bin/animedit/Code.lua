function SwitchPanel(type)
	if visible == "bones" then
		SendMessage("spritespanel", "show")
		visible = "sprites"
	elseif visible == "sprites" then
		SendMessage("spritespanel", "hide");
		visible = "bones"
	end
end;

visible = "bones"
SendMessage("radio", "clear");
SendMessage("radio", "add bones");
SendMessage("radio", "add sprites");
currentTexture = "null";
