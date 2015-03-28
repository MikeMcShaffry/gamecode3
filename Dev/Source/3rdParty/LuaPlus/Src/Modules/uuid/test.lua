require "uuid"

-- test uuid library

print(uuid.version)
print""

u=uuid.new()
print(u)

u=uuid.new()
print(u)

r=uuid.new()
print(r)

t=uuid.new()
print(t)

t=uuid.new()
print(t)
print""

function test(x,ok)
 print(x,uuid.isvalid(x),ok)
end

test("{84949cc5-4701-4a84-895b-354c584a981b}", true)
test("{84949CC5-4701-4A84-895B-354C584A981B}", true)
test("{84949cc5-4701-4a84-895b-354c584a981bc}", false)
test("{84949cc5-4701-4a84-895b-354c584a981}", false)
test("{84949cc5x4701-4a84-895b-354c584a981b}", false)
test("{84949cc504701-4a84-895b-354c584a981b}", false)
test("{84949cc5-470104a84-895b-354c584a981b}", false)
test("{84949cc5-4701-4a840895b-354c584a981b}", false)
test("{84949cc5-4701-4a84-895b0354c584a981b}", false)
test("{g4949cc5-4701-4a84-895b-354c584a981b}", false)
test("{84949cc5-4701-4a84-895b-354c584a981g}", false)

print""
print(uuid.version)
