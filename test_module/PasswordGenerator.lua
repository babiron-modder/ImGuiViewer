
-- フラグ
f_number = true
f_english_small = true
f_english_large = true
f_symbol = {
  {false , "!" , "!" },
  {false , "\"", "\""},
  {false , "#" , "#" },
  {false , "$" , "$" },
  {false , "%" , "%" },
  {false , "&" , "&" },
  {false , "'" , "'" },
  {false , "(" , "(" },
  {false , ")" , ")" },
  {false , "*" , "*" },

  {false , "+" , "+" },
  {false , "," , "," },
  {false , "-" , "-" },
  {false , "." , "." },
  {false , "/" , "/" },
  {false , ":" , ":" },
  {false , ";" , ";" },
  {false , "<" , "<" },
  {false , "=" , "=" },
  {false , ">" , ">" },

  {false , "?" , "?" },
  {false , "@" , "@" },
  {false , "[" , "[" },
  {false , "\\", "\\"},
  {false , "]" , "]" },
  {false , "^" , "^" },
  {false , "_" , "_" },
  {false , "`" , "`" },
  {false , "{" , "{" },
  {false , "|" , "|" },

  {false , "}" , "}" },
  {false , "~" , "~" },
  {false , "\\x020" , " " }
}

f_aimai = false
f_hissu = true

TABLE_COLUMN = 6
password_len = 16

eng_small = {
  {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"},
  {"a", "b", "c", "d", "e", "f", "g", "h", "j", "k", "m", "n", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"}
}
eng_large = {
  {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"},
  {"A", "B", "C", "D", "E", "F", "G", "H", "J", "K", "L", "M", "N", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"}
}

generated_text = ""


function shuffle(_text)
  local shuffled_text = ""
  local temp_text = _text
  local next_text = ""
  for i = #_text, 1, -1 do
    local rand = math.random(1, i)
    shuffled_text = shuffled_text..string.sub(temp_text, rand, rand)
    next_text = ""
    if(rand~=1) then
      next_text = string.sub(temp_text, 1, rand-1)
    end
    if(rand~=i) then
      next_text = next_text..string.sub(temp_text, rand+1, i)
    end
    temp_text = next_text
  end
  return shuffled_text
end

function Loop()
  imBegin("Password Generator")

  imText("＜基本条件＞")

  imBeginTable("##table", 3, IMLUA_TABLE_BORDERS)
  imTableNextColumn()
  f_number = imCheckBox("数字", f_number)
  imTableNextColumn()
  f_english_small = imCheckBox("英語小文字", f_english_small)
  imTableNextColumn()
  f_english_large = imCheckBox("英語大文字", f_english_large)
  imEndTable()

  imText("＜記号＞")
  if(imTreeNode("##記号")) then
    imUnindent()

    imBeginTable("##symbol table", TABLE_COLUMN, IMLUA_TABLE_BORDERS)

    for i = 1, #f_symbol do
      imTableNextColumn()
      f_symbol[i][1] = imCheckBox(f_symbol[i][2], f_symbol[i][1])
    end
    imEndTable()

    imTreePop()
    imIndent()
  end

  imText("＜その他条件＞")
  imBeginTable("##その他条件", 1, IMLUA_TABLE_BORDERS)
  imTableNextColumn()
  f_aimai = imCheckBox("「i」,「l」, 「I」,「1」,「|」,「o」,「0」,「O」 を除く", f_aimai)
  --imTableNextRow()
  imTableNextColumn()
  f_hissu = imCheckBox("チェックした項目を必ず使用する", f_hissu)
  if(f_aimai) then
    f_symbol[30][1] = false
  end
  imEndTable()


  imDummy(0,10)


  imPushItemWidth(-1)

  imText("パスワードの長さ: ")
  imSameLine()
  password_len = imSliderInt("パスワード長", password_len, 1, 32)

  imDummy(0,10)


  if(imButton("生成", -1, 0)) then
    local hissu = {}
    local kouho = {}
    generated_text = ""
    kouho_char = ""
    if(f_number) then
      if(f_aimai) then
        hissu[#hissu+1] = ""..math.random(2, 9)
        for i = 2, 9 do
          kouho_char = kouho_char..i
        end
      else
        hissu[#hissu+1] = ""..math.random(0, 9)
        for i = 0, 9 do
          kouho_char = kouho_char..i
        end
      end
    end
    if(f_english_small) then
      if(f_aimai) then
        hissu[#hissu+1] = eng_small[2][math.random(1, #eng_small[2])]
        for i = 1, #eng_small[2] do
          kouho_char = kouho_char..eng_small[2][i]
        end
      else
        hissu[#hissu+1] = eng_small[1][math.random(1, #eng_small[1])]
        for i = 1, #eng_small[1] do
          kouho_char = kouho_char..eng_small[1][i]
        end
      end
    end
    if(f_english_large) then
      if(f_aimai) then
        hissu[#hissu+1] = eng_large[2][math.random(1, #eng_large[2])]
        for i = 1, #eng_large[2] do
          kouho_char = kouho_char..eng_large[2][i]
        end
      else
        hissu[#hissu+1] = eng_large[1][math.random(1, #eng_large[1])]
        for i = 1, #eng_large[1] do
          kouho_char = kouho_char..eng_large[1][i]
        end
      end
    end
    for i = 1, #f_symbol do
      if(f_symbol[i][1]) then
        hissu[#hissu+1] = f_symbol[i][3]
        kouho_char = kouho_char..f_symbol[i][3]
      end
    end --必須処理

    if (f_hissu) then
      for i = 1, #hissu do
        generated_text = generated_text..hissu[i]
      end
    end

    for i = #generated_text, password_len-1 do
      local rand = math.random(1, #kouho_char)
      generated_text = generated_text..string.sub(kouho_char, rand, rand)
    end

    generated_text = shuffle(generated_text)
    generated_text = string.sub(generated_text, 1, password_len)


  end

  imSeparator()
  imDummy(0,10)

  imText("生成したパスワード: ")
  imSameLine()
  generated_text = imInputText("##outtext", generated_text, #generated_text+1)

  imPopItemWidth()

  imEnd()
end
