aa="poo"

function Loop()
  imBegin("hoge")
  if(imButton("change")==1) then
    aa="hoo"
  end
  imButton(aa)
  imButton("huga")
  imEnd()
end

