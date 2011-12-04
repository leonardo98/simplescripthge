local file = OpenFile("C:\\", "Images: OGG files\0*.ogg\0Images: PNG files\0*.png\0Images: JPG files\0*.jpg\0All files\0*.*\0\0");
if file ~= "null" then
	SendMessage("video", "set "..file);
    SendMessage("video", "play");
end