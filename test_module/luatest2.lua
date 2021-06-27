aa="poo"
boo=true
coo=flase
tmp_text = "hoge"
radio_select = 0

a=23
c=2
d=20

b = 1
bb = {1, 2}
bbb = {1, 2, 3}
bbbb = {1, 2, 3, 4}

progress = 0.5

items = {"aaaa", "bbbb", "cccc", "dddd", "eeee", "ffff"}
selected_item = 1


function Loop()
  imBegin("Test!!")


  if(imCollapsingHeader("CollapsingHeader!")) then

  end
  if(imTreeNode("Node1")) then
    imText("なんとかかんとか")
    imText("なんとかかんとか")
    imTreePop()
  end

  imSeparator()
  selected_item = imCombo("imCombo", selected_item, items, #items)
  imText("selected["..selected_item.."]: "..items[selected_item])


  imSeparator()
  selected_item = imListBox("imListBox", selected_item, items, #items, 3)
  imSeparator()


  if(imBeginTable("imBeginTable", 3)) then
    imTableNextRow()
    imTableNextColumn()

    imText("id:")

    imTableNextColumn()

    imInputText("##iminputtexthoge", tmp_text, 20)

    imTableNextColumn()

    imText("sss")

    imTableNextRow()
    imTableNextColumn()

    imText("sss")
    imPushItemWidth(-1)
    selected_item = imCombo("imCombo2", selected_item, items, #items)
    imPopItemWidth()

    imTableNextColumn()

    imText("hoge")

    imTableNextRow()
    imTableNextColumn()
    imTableNextColumn()
    imTableNextColumn()

    imText("hoge")

    imEndTable()
  end


  imSeparator()
  imText("tooltip")
  if(imIsItemHovered()) then
    imSetTooltip("This is Tool Tip")
  end

  imSeparator()
  imProgressBar(progress, progress.."/1.000")
  progress = imSliderFloat("progress", progress, 0, 1)
  imSeparator()


  imSeparator()


  imBulletText("imRadioButton")
  radio_select = imRadioButton("radio1", radio_select, 1)
  imSameLine()
  radio_select = imRadioButton("radio2", radio_select, 2)
  imSameLine()
  radio_select = imRadioButton("radio3", radio_select, 3)


  imSeparator()
  if (imButton(aa)) then
    aa = "piyo"
    boo = not boo
  end
  imSameLine()
  if(imButton("change")) then
    aa="hoo"
  end
  imSmallButton("imSmallButton");
  imText(tostring(boo))
  boo = imCheckBox("checkbox", boo)
  if(boo) then
    imText("boooooo!")
  end
  coo = imCheckBox("checkbox2", coo)
  imPushID("miso")
  coo = imCheckBox("checkbox2", coo)
  imPopID()

  b = imInputInt("imInputInt", b)
  b = imSliderInt("imSliderInt", b, -12, 18)
  b = imDragInt("imDragInt", b, 0.1 , -12, 18)
  bb = imInputInt2("imInputInt2", bb)
  bb = imSliderInt2("imSliderInt2", bb, -12, 18)
  bb = imDragInt2("imDragInt2", bb , 0.1 , -12, 18)
  bbb = imInputInt3("imInputInt3", bbb)
  bbb = imSliderInt3("imSliderInt3", bbb, -12, 18)
  bbb = imDragInt3("imDragInt3", bbb , 0.1 , -12, 18)
  bbbb = imInputInt4("imInputInt4", bbbb)
  bbbb = imSliderInt4("imSliderInt4", bbbb, -12, 18)
  bbbb = imDragInt4("imDragInt4", bbbb , 0.1 , -12, 18)


  tmp_text = imInputText("imInputText", tmp_text, 10)

  imText("-> "..tmp_text)
  if(tmp_text == "hoge") then
    imText("is hoge !!")
  end
  imEnd()
end
