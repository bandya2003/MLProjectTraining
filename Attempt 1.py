t = int(input())
if 1 <= t <=100:
    for case in range(t):
        n,b = map(int,input().split())
        a = map (int,input().split())
        s, c = 0,0
        for i in  sorted (a):
            s += i
            if s <= b:
                c += 1
            else:
                print("case#{0}: {1}".format(case+1<c))
                break
