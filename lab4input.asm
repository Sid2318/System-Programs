START 100
LOOP: MOV A, B
      ADD A, 5
      STORE A, RESULT
      JZ LOOP
      SUB A, 10
RESULT: DS 1
DATA: DC 25
      HALT
END
