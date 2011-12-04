
-- Level 1

function Level1Init()
	-- level1stage = 0

	cactus = "on"
	clock = "one"
	shtora1 = "one"
	shtora2 = "one"

	l1_bullfinch = ""
	tuk = ""
	l1_bobcat = "one"
	door = "one"
	babl = ""
end

function Level1TukTuk() 
 	 if (level1stage == 2) then
	     ArrowClick()
     end
end

function Level1Cactus()
	if cactus == "on" then
		cactus = "off"
	else
		cactus = "on"
	end
end

function Level1Switch()
	if (level1stage == 0) then
		SendMessage("messageWindow" , "Рысенок проснулся рано утром и посмотрел в окно.\nЗа окном кружились снежинки, кругом лежал снег...\n - Вот и пришла зима - подумал Рысенок." )
		l1_bullfinch = ""
		tuk = ""
		l1_bobcat = "one"
		door = "one"
		babl = ""
	elseif (level1stage == 1) then
		SendMessage("messageWindow" , "Вдруг кто-то постучал в дверь.\nИнтересно, кто это пришел в гости к Рысенку с утра по раньше." )
		l1_bullfinch = ""
		door = "one"
		babl = ""
		tuk = "one"
		l1_bobcat = "two"
	elseif (level1stage == 2) then
		SendMessage("messageWindow" , "Да это же его лучший друг Снегирь.\nРысенок очень обрадовался его приходу." )
		babl = ""
		tuk = ""
		door = "two"
		l1_bullfinch = "one"
		l1_bobcat = "three"
	elseif (level1stage == 3) then
		SendMessage("messageWindow" , "- Чем бы нам сегодня заняться? - спросил Рысенок у Снегиря.\n - А давай слепим снеговика - предложил Снегирь." )
		tuk = ""
		babl = ""
		l1_bullfinch = "two"
		l1_bobcat = "four"
		door = "one"
	elseif (level1stage == 4) then
		SendMessage("messageWindow" , "Отличная идея! Но прежде чем Рысенок и снегирь\nпойдут лепить снеговикаим нужно найти дома\nвсе необходимое для этого. Что же им пригодится?" )
		tuk = ""
		door = "one"
		babl = ""
		l1_bullfinch = "three"
		l1_bobcat = "five"
	elseif (level1stage == 5) then
		SendMessage("messageWindow" , "Друзья подумали и решили найти дома:\nодну морковку, шарф, ведро, метлу и 2 пуговицы." )
		l1_bullfinch = "three"
		tuk = ""
		l1_bobcat = "five"
		door = "one"
		babl = "one"
	end
end

function Level1Next()
	if (level1stage < 5) then
		level1stage = level1stage + 1
		Level1Switch()
	else
		level2stage = 0
		SendMessage("Core", "load xml Level2.xml")
		Level2Init();
	end
end
	
function Level1Prev()
	if (level1stage > 0) then
		level1stage = level1stage - 1
		Level1Switch()
	else
		SendMessage("Core", "load xml start2.xml")
	end
end
 

-- Level 2

function Level2Init()
	--level2stage = 0
	chair = "one"
	shtora = "one"
	l2_bullfinch = "one"
	l2_bobcat = "one"
	icebox = "one"
end

function Level2Switch()
    if (level2stage == 0) then
		level2stage = 0
		SendMessage("messageWindow" , "Интересно, что Рысенок и Снегирь могут найти здесь?" )
		l2_bullfinch = "one"
		l2_bobcat = "one"
		icebox = "one"
    elseif (level2stage == 1) then
		SendMessage("messageWindow" , "Конечно же в холодильнике нашлась морковка." )
		l2_bullfinch = "two"
		icebox = "two"
		l2_bobcat = "two"
    elseif (level2stage == 2) then
		level2stage = 2
		SendMessage("messageWindow" , "Она как раз подойдет для того, чтобы сделать нос снеговику." )
		icebox = "three"
		l2_bullfinch = "two"
		l2_bobcat = "two"
    end
end

function Level2Next()
	if (level2stage < 2) then
		level2stage = level2stage + 1
		Level2Switch()
	else
		closet2stage = 0
		SendMessage("Core", "load xml closet2.xml")
		Closet2Init()	
	end
end

function Level2Prev()
	if (level2stage > 0) then
		level2stage = level2stage - 1
		Level2Switch()
	else
		level1stage = 5
		SendMessage("Core", "load xml Level1.xml")
		Level1Init()
	end
end


-- closet2

function Closet2Init()
	--closet2stage = 0
	picture = "one"
        bucket = "one"
        switch = "off"
        broom = "one"
        cheese = "one"
        mouse = ""
	l3_bullfinch = "one"
	l3_bobcat = "one"
end

function Closet2Switch()
    if (closet2stage == 0) then
		closet2stage = 0
		SendMessage("messageWindow" , "В кладовке то же можно найти что-то полезное,\nно вней было так темно, что Снегирь нечего не мог разглядеть.\n- Нужно включить свет - подумал Снегирь." )
		l3_bullfinch = "one"
                switch = "off"
		l3_bobcat = "one"
                bucket = "one"
                broom = "one"
    elseif (closet2stage == 1) then
		SendMessage("messageWindow" , "Со светом все стало видно, тут как раз есть, то что им нужно.\nВедро стояло на стуле,\nа рядом лежала старенькая метла." )
		l3_bullfinch = "two"
                switch = "on"
                bucket = "one"
                broom = "one"
		l3_bobcat = "two"
    elseif (closet2stage == 2) then
		SendMessage("messageWindow" , "Ведро пригодится, что бы надеть его снеговику на голову." )
		l3_bullfinch = "two"
                bucket = ""
                broom = "one"
		l3_bobcat = "two"
    elseif (closet2stage == 3) then
		SendMessage("messageWindow" , "Метла те же пригодится для лепки снеговика." )
                bucket = ""
                broom = ""
		l3_bullfinch = "two"
		l3_bobcat = "two"
    end
end

function Closet2Next()
	if (closet2stage < 3) then
		closet2stage = closet2stage + 1
		Closet2Switch()
	else
		cabinetstage = 0
		SendMessage("Core", "load xml cabinet.xml")
		--CabinetInit()	
	end
end

function Closet2Prev()
	if (closet2stage > 0) then
		closet2stage = closet2stage - 1
		Closet2Switch()
	else
		closet2stage = 3
		SendMessage("Core", "load xml Level2.xml")
		Level2Init()
	end
end


-- cabinet

function CabinetInit()
	--cabinetstage = 0
	l4_bullfinch = "one"
	l4_bobcat = "one"
	cabinet = "one"
end

function CabinetSwitch()
    if (cabinetstage == 0) then
		cabinetstage = 0
		SendMessage("messageWindow" , "В комнате у Рысенка стоял старый синий шкаф для одежды. Он достался ему от дедушки, который был великолепным плотником. Снегирь сразу решил посмотреть, что там внутри." )
		l4_bullfinch = "one"
		l4_bobcat = "one"
		cabinet = "one"
    elseif (cabinetstage == 1) then
		SendMessage("messageWindow" , "В шкафу нашелся полосатый, старенький шарфик." )
		l4_bullfinch = "two"
		cabinet = "two"
		l4_bobcat = "two"
    elseif (cabinetstage == 2) then
		SendMessage("messageWindow" , "Он как раз подойдет для того, чтобы украсить снеговика." )
		cabinet = "three"
		l4_bullfinch = "two"
		l4_bobcat = "two"
    end
end

function CabinetNext()
	if (cabinetstage < 2) then
		cabinetstage = cabinetstage + 1
		CabinetSwitch()
	else
		casketstage = 0
		SendMessage("Core", "load xml casket.xml")
		CasketInit()	
	end
end

function CabinetPrev()
	if (cabinetstage > 0) then
		cabinetstage = cabinetstage - 1
		CabinetSwitch()
	else
		closet2stage = 3
		SendMessage("Core", "load xml closet2.xml")
		Closet2Init()
	end
end


-- casket

function CasketInit()
	--casketstage = 0
	l5_bobcat = "one"
	casket = "one"
end

function CasketSwitch()
    if (casketstage == 0) then
		casketstage = 0
		SendMessage("messageWindow" , "- Кажется я знаю, что найду здесь\n - подумал Рысенок и подошел к шкатулке." )
		l5_bobcat = "one"
		casket = "one"
    elseif (casketstage == 1) then
		SendMessage("messageWindow" , "Конечно же в шкатулке нашлись 2 большие пуговицы.\n Их то и искал Рысенок." )
		casket = "two"
		l5_bobcat = "two"
    elseif (casketstage == 2) then
		SendMessage("messageWindow" , "Из пуговиц получатся отличные глаза для снеговика." )
		casket = "three"
		l5_bobcat = "two"
    end
end

function CasketNext()
	if (casketstage < 2) then
		casketstage = casketstage + 1
		CasketSwitch()
	else
		room_2stage = 0
		SendMessage("Core", "load xml room_2.xml")
		Room_2Init()	
	end
end

function CasketPrev()
	if (casketstage > 0) then
		casketstage = casketstage - 1
		CasketSwitch()
	else
		cabinetstage = 3
		SendMessage("Core", "load xml cabinet.xml")
		CabinetInit()
	end
end


-- room_2

function Room_2Init()
	--room_2stage = 0
	l6_bobcat = "one"
        l6_bullfinch = "one"
	street_room2 = "one"
        clock = "one"
        door = "one"
end

function Room_2Switch()
    if (room_2stage == 0) then
		room_2stage = 0
		SendMessage("messageWindow" , "Теперь все готово для того,\n что бы идти на улицу и лепить снеговика!" )
		l6_bobcat = "one"
                l6_bullfinch = "one"
                door = "one"
		street_room2 = "one"
    elseif (room_2stage == 1) then
		SendMessage("messageWindow" , "Рысенок и Снегирь взяли собранные вещи и пошли на улицу." )
		street_room2 = "two"
                l6_bullfinch = "one"
                l6_bobcat = "one"
                door = "two"
    end
end

function Room_2Next()
	if (room_2stage < 1) then
		room_2stage = room_2stage + 1
		Room_2Switch()
	else
		shemastage = 0
		SendMessage("Core", "load xml shema.xml")
		ShemaInit()	
	end
end

function Room_2Prev()
	if (room_2stage > 0) then
		room_2stage = room_2stage - 1
		Room_2Switch()
	else
		casketstage = 2
		SendMessage("Core", "load xml casket.xml")
		CasketInit()
	end
end


-- shema

function ShemaInit()
	--shemastage = 0
end

function ShemaSwitch()
    if (shemastage == 0) then
		shemastage = 0
    end
end

function ShemaNext()
	if (shemastage < 0) then
		shemastage = shemastage + 1
		ShemaSwitch()
	else
		streetstage = 0
		SendMessage("Core", "load xml street.xml")
		StreetInit()	
	end
end

function ShemaPrev()
	if (shemastage > 0) then
		shemastage = shemastage - 1
		ShemaSwitch()
	else
		room_2stage = 1
		SendMessage("Core", "load xml room_2.xml")
		Room_2Init()
	end
end



-- street

function StreetInit()
	--streetstage = 0
	l8_bullfinch = "one"
	l8_bobcat = "one"
        bobcat_eyes = ""
        comok = ""
	comok2 = ""
        comok3 = ""
        comok_carrot = ""
        comok_eyes = ""
        comok_bucket = ""
        comok_broom = ""
        comok_scarf = ""

end

function StreetSwitch()
    if (streetstage == 0) then
		streetstage = 0
		SendMessage("messageWindow" , "Друзья вышли на улицу и принялись за дело." )
		l8_bullfinch = "one"
                l8_bobcat = "one"
                bobcat_eyes = ""
                comok2 = ""
                comok3 = ""
                comok_carrot = ""
                comok = ""
                comok_eyes = ""
                comok_bucket = ""
                comok_scarf = ""
                comok_broom = ""
    elseif (streetstage == 1) then
		SendMessage("messageWindow" , "Рысенок сделал 2 снежных комка один большой, а другой поменьше. Получилось туловище." )
		comok2 = "one"
                bobcat_eyes = "one"
                l8_bullfinch = "one"
                l8_bobcat = "one"
                comok3 = ""
                comok_carrot = ""
                comok = ""
                comok_eyes = ""
                comok_bucket = ""
                comok_scarf = ""
                comok_broom = ""
   elseif (streetstage == 2) then
		SendMessage("messageWindow" , "Затем на туловище друзья поставили небольшой комочек из которого получилась голова." )
		comok3 = "one"
                l8_bullfinch = "one"
                bobcat_eyes = ""
                l8_bobcat = "one"
                comok2 = "one"
                comok_carrot = ""
                comok = ""
                comok_eyes = ""
                comok_bucket = ""
                comok_scarf = ""
                comok_broom = ""
   elseif (streetstage == 3) then
		SendMessage("messageWindow" , "Из двух маленьких комочков Рысенок сделал снеговику руки. А Снегирь в это время приготовил морковку, чтобы сделать снеговику нос." )
		l8_bullfinch = "seven"
                comok = "one"
                l8_bobcat = "two"
                bobcat_eyes = ""
                comok2 = ""
                comok3 = ""
                comok_carrot = ""
                comok_eyes = ""
                comok_bucket = ""
                comok_scarf = ""
                comok_broom = ""
   elseif (streetstage == 4) then
		SendMessage("messageWindow" , "Нос из морковки получился великолепный." )
		 l8_bullfinch = "two"
                 comok_carrot = "one"
                 l8_bobcat = "seven"
                 bobcat_eyes = ""
                 comok2 = ""
                 comok3 = ""
                 comok = "one"
                 comok_eyes = ""
                 comok_bucket = ""
                 comok_scarf = ""
                 comok_broom = ""
   elseif (streetstage == 5) then
		SendMessage("messageWindow" , "Рысенок прилепил к голове снеговика пуговицы, из них получились глаза." )
		 l8_bullfinch = "three"
                 l8_bobcat = "three"
                 comok_eyes = "one"
                 bobcat_eyes = ""
                 comok2 = ""
                 comok3 = ""
                 comok_carrot = "one"
                 comok = "one"
                 comok_bucket = ""
                 comok_scarf = ""
                 comok_broom = ""
    elseif (streetstage == 6) then
		SendMessage("messageWindow" , "Снегирь взлетел и одел снеговику ведро на голову." )
		 l8_bullfinch = "four"
                 l8_bobcat = "four"
                 comok_bucket = "one"
                 comok_eyes = "one"
                 bobcat_eyes = ""
                 comok2 = ""
                 comok3 = ""
                 comok_carrot = "one"
                 comok = "one"
                 comok_scarf = ""
                 comok_broom = ""
     elseif (streetstage == 7) then
		SendMessage("messageWindow" , "А Рысенок намотал снеговику на шею шарфик, чтобы он не замерз. Теперь осталось влепить в руку снеговику метлу." )
		 l8_bullfinch = "five"
                 l8_bobcat = "five"
                 comok_scarf = "one"
                 comok_eyes = "one"
                 bobcat_eyes = ""
                 comok2 = ""
                 comok3 = ""
                 comok_carrot = "one"
                 comok = "one"
                 comok_bucket = "one"
                 comok_broom = ""
      elseif (streetstage == 8) then
		SendMessage("messageWindow" , "Ура! Все готово!" )
		 l8_bullfinch = "six"
                 l8_bobcat = "six"
                 comok_broom = "one"
                 comok_eyes = "one"
                 bobcat_eyes = ""
                 comok2 = ""
                 comok3 = ""
                 comok_carrot = "one"
                 comok = "one"
                 comok_bucket = "one"
                 comok_scarf = "one"
    end
end

function StreetNext()
	if (streetstage < 8) then
		streetstage = streetstage + 1
		StreetSwitch()
	else
		conetzstage = 0
		SendMessage("Core", "load xml conetz.xml")
		ConetzInit()	
	end
end

function StreetPrev()
	if (streetstage > 0) then
		streetstage = streetstage - 1
		StreetSwitch()
	else
		shemastage = 1
		SendMessage("Core", "load xml shema.xml")
		ShemaInit()
	end
end


-- conetz

function ConetzInit()
	--conetzstage = 0
        l9_bullfinch = "one"
	l9_bobcat = "one"
        babl = ""
end

function ConetzSwitch()
    if (conetzstage == 0) then
        SendMessage("messageWindow" , "Отличный у нас получился снеговик! \n“Давай завтра что-нибудь еще придумаем”,\n- предложил Рысенок. “Давай”,- согласился Снегирь. " )
        l9_bullfinch = "one"
	l9_bobcat = "one"
        babl = ""		
 elseif (conetzstage == 1) then
	l9_bullfinch = "one"
	l9_bobcat = "one"
        babl = "one"	
    end
end

function ConetzNext()
	if (conetzstage < 1) then
		conetzstage = conetzstage + 1
		ConetzSwitch()
	else
		start2stage = 0
		SendMessage("Core", "load xml start2.xml")
		Start2Init()	
	end
end

function ConetzPrev()
	if (conetzstage > 0) then
		conetzstage = conetzstage - 1
		ConetzSwitch()
	else
		streetstage = 8
		SendMessage("Core", "load xml street.xml")
		StreetInit()
	end
end
