# python 3.6.8
# Jimmyyao
# use numba jit to speedup
# use multiprocessor
import sys
import numpy as np
from numba import jit
from concurrent.futures import ProcessPoolExecutor

mismatch={'--':0,'A-':1,'T-':2,'G-':1,'C-':3,
            '-A':1,'AA':0,'TA':1,'GA':5,'CA':1,
            '-T':2,'AT':1,'TT':0,'GT':9,'CT':1,
            '-G':1,'AG':5,'TG':9,'GG':0,'CG':1,
            '-C':3,'AC':1,'TC':1,'GC':1,'CC':0,
            }

# forward get path array to find q (using dp)
# reverse string it will be g

@jit(nopython=True,forceobj=True)
def f(s1, s2):
  m=len(s1)
  n=len(s2)
  dp=np.full(m+1,sys.maxsize)
  dp[0]=0
  for i in range(1,m+1):
      dp[i]=mismatch[s1[i-1]+'-']+dp[i-1]
  for j in range(1,n+1):
      nextuse=dp[0]
      dp[0]=dp[0]+mismatch['-'+s2[j-1]]
      for i in range(1,m+1):
          temp=dp[i]
          dp[i]=min(mismatch[s1[i-1]+s2[j-1]]+nextuse,mismatch['-'+s2[j-1]]+dp[i])
          dp[i]=min(dp[i],mismatch[s1[i-1]+'-']+dp[i-1])
          nextuse=temp
  return dp

@jit(nopython=True,forceobj=True)
def get_q(left, right):
  lr=left+right[::-1]
  return np.argmin(lr)

@jit(nopython=True,forceobj=True)
def dynamicProgramming(x, y):
  # x is column
  # y is row
  dp = np.full((len(x) + 1, len(y) + 1),sys.maxsize)
  path = np.empty((len(x) + 1, len(y) + 1), dtype=object)

  dp[0][0]=0
  for i in range(1, len(x) + 1):
    dp[i][0] = dp[i-1][0] + mismatch[x[i-1]+'-']
    path[i][0] = "u"
  for j in range(1, len(y) + 1):
    dp[0][j] = dp[0][j-1] + mismatch['-'+y[j-1]]
    path[0][j] = "l"
  
  for i in range(1, len(x) + 1):
    for j in range(1, len(y) + 1):
      # 3 choices
      c1=mismatch[x[i-1]+y[j-1]]+dp[i-1][j-1]
      c2=mismatch['-'+y[j-1]]+dp[i-1][j]
      c3=mismatch[x[i-1]+'-']+dp[i][j-1]
      c=min(min(c1,c2),c3)
      if(c==c1):
        path[i][j]="d"
      elif(c==c2):
        path[i][j]="u"
      else:
        path[i][j]="l"
      dp[i][j]=c
  row = []
  column= []
  i = len(x)
  j = len(y)
  while path[i][j]:
    if path[i][j] == "d":
      row.insert(0, y[j-1])
      column.insert(0, x[i-1])
      i -= 1
      j -= 1
    elif path[i][j] == "u":
      row.insert(0, '-')
      column.insert(0, x[i-1])
      i -= 1
    elif path[i][j] == "l":
      column.insert(0, '-')
      row.insert(0, y[j-1])
      j -= 1
  return row, column


def Hirschberge(s1, s2):
  row = ""
  column = ""
  cost=0
#  s2 is being row
#  s1 is being column
  if len(s1) == 0 or len(s2) == 0:
    if len(s1) == 0:
      column = '-' * len(s2)
      row =s2
    else:
      column = s1
      row += '-' * len(s1)
  elif len(s1) == 1 or len(s2) == 1:
    row,column= dynamicProgramming(s1, s2)
    row,column= map(lambda x: "".join(x), [row,column])
  else:
    n = len(s2)
    s2mid = (int)(n/2)
    m = len(s1)

    left = f(s1,s2[:s2mid])
    right = f(s1[::-1],s2[s2mid:][::-1])
    #print(scoreL)
    #print(scoreR)
    s1mid = get_q(left,right)
    cost=(left+right[::-1])[s1mid]
    row_l, column_u,_ = Hirschberge(s1[:s1mid], s2[:s2mid])
    row_r, column_d,_ = Hirschberge(s1[s1mid:], s2[s2mid:])
    row = row_l + row_r
    column = column_u + column_d 

  return row, column,cost
        


def main():
  s1=open("Data-P2a.txt","r")
  s1=s1.read()
  s2=open("Data-P2b.txt","r")
  s2=s2.read()
  s1=s1[:-1]
  s2=s2[:-1]
  row, column,cost=Hirschberge(s1,s2)
  out=open("Answer-P2.txt",'w')
  out.write("cost is {}".format(cost))
  out.close()
  out=open("Answer-P2a.txt","w")
  out.write(column)
  out.close()
  out=open("Answer-P2b.txt","w")
  out.write(row)
  out.close()


if __name__=="__main__":
  with ProcessPoolExecutor(max_workers=40) as executor:
    executor.map(main())
