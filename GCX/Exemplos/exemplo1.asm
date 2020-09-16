; DPonto
     ; P1
     STIF  #-10.0000 , 0(DS)
     STIF  #10.0000 , 2(DS)
     STIF  #10.0000 , 4(DS)
     ; P2
     STIF  #-10.0000 , 6(DS)
     STIF  #-10.0000 , 8(DS)
     STIF  #10.0000 , 10(DS)
     ; P3
     STIF  #10.0000 , 12(DS)
     STIF  #-10.0000 , 14(DS)
     STIF  #10.0000 , 16(DS)
     ; P4
     STIF  #10.0000 , 18(DS)
     STIF  #10.0000 , 20(DS)
     STIF  #10.0000 , 22(DS)
     ; P5
     STIF  #-10.0000 , 24(DS)
     STIF  #10.0000 , 26(DS)
     STIF  #-10.0000 , 28(DS)
     ; P6
     STIF  #-10.0000 , 30(DS)
     STIF  #-10.0000 , 32(DS)
     STIF  #-10.0000 , 34(DS)
     ; P7
     STIF  #10.0000 , 36(DS)
     STIF  #-10.0000 , 38(DS)
     STIF  #-10.0000 , 40(DS)
     ; P8
     STIF  #10.0000 , 42(DS)
     STIF  #10.0000 , 44(DS)
     STIF  #-10.0000 , 46(DS)
; DCor
     ; AZUL
     STI   #15 , 48(DS)
     STI   #20 , 49(DS)
     STI   #20 , 50(DS)
     STI   #40 , 51(DS)
     STI   #30 , 52(DS)
     STI   #30 , 53(DS)
     STI   #60 , 54(DS)
; DFace
     ; F1
     STI   #4 , 55(DS)  ; Num de pontos 
     STI   #48 , 56(DS) ; End da Cor
     STI   #0 , 57(DS)  ; End de P1
     STI   #6 , 58(DS)  ; End de P2
     STI   #12 , 59(DS) ; End de P3
     STI   #18 , 60(DS) ; End de P4
     ; F2
     STI   #4 , 61(DS)
     STI   #48 , 62(DS)
     STI   #18 , 63(DS)
     STI   #12 , 64(DS)
     STI   #36 , 65(DS)
     STI   #42 , 66(DS)
     ; F3
     STI   #4 , 67(DS)
     STI   #48 , 68(DS)
     STI   #42 , 69(DS)
     STI   #36 , 70(DS)
     STI   #30 , 71(DS)
     STI   #24 , 72(DS)
     ; F4
     STI   #4 , 73(DS)
     STI   #48 , 74(DS)
     STI   #24 , 75(DS)
     STI   #30 , 76(DS)
     STI   #6 , 77(DS)
     STI   #0 , 78(DS)
     ; F5
     STI   #4 , 79(DS)
     STI   #48 , 80(DS)
     STI   #24 , 81(DS)
     STI   #0 , 82(DS)
     STI   #18 , 83(DS)
     STI   #42 , 84(DS)
     ; F6
     STI   #4 , 85(DS)
     STI   #48 , 86(DS)
     STI   #36 , 87(DS)
     STI   #12 , 88(DS)
     STI   #6 , 89(DS)
     STI   #30 , 90(DS)
; DLuz
     ; L1
     STIF  #200.0000 , 91(DS)
     STIF  #200.0000 , 93(DS)
     STIF  #200.0000 , 95(DS)
; DObjeto
     ; CUBO
     STI   #6 , 97(DS)       ; Num de Faces
     STIF  #1.0000 , 98(DS)  ; Escala do Objeto
     STI   #55 , 100(DS)     ; End F1
     STI   #61 , 101(DS)     ; End F2
     STI   #67 , 102(DS)     ; End F3
     STI   #73 , 103(DS)     ; End F4
     STI   #79 , 104(DS)     ; End F5
     STI   #85 , 105(DS)     ; End F6
; DConst
     STIF  #1.5000 , 106(DS)  ; ESC
     STI   #50 , 108(DS)      ; MAXTEMPO
     STIF  #0.0700 , 109(DS)  ; G
     STI   #-200 , 111(DS)    ; D0X
     STI   #200 , 112(DS)     ; D0Y
     STI   #0 , 113(DS)       ; D0Z
     STI   #8 , 114(DS)       ; V0X
     STI   #2 , 115(DS)       ; V0Y
     STI   #0 , 116(DS)       ; V0Z
************************** Corpo Programa ******
     LDI   A , #91  ; REG <- End de Luz
     LGT   A        ; LuzCor <- REG
; Escala
     LODF  A , 106(DS) ; REG <- End de Const-ESC
     LDI   A , #97     ; REG <- End de Cubo
     ESC   A , A       ; REG1 -> escala <- REG2
; Atribuicao: i=1
     STI   #1 , 118(DS) ; 1 -> End
     LOD   A , 118(DS)  ; REG <- End da EXP
     STO   A , 117(DS)  ; REG -> End de Var-i
; Rottrans
************************** Expressao ******
     STI   #0 , 118(DS)
************************** Expressao ******
     STI   #0 , 119(DS)
************************** Expressao ******
     STI   #0 , 120(DS)
************************** Expressao ******
************************** Expressao ******
************************** Expressao ******
     LOD   A , 118(DS)
     CNV   A , A
     LOD   B , 119(DS)
     CNV   B , B
     LOD   C , 120(DS)
     CNV   C , C
     LOD   D , 111(DS)
     CNV   D , D
     LOD   E , 112(DS)
     CNV   E , E
     LOD   F , 113(DS)
     CNV   F , F
     LDI   A , #97     ; REG <- End de Cubo
     RTR 
; Enquanto 
ROT1: ; Rot de inicio do enquanto
************************** Expressao ******
************************** Expressao ******
     LOD   A , 117(DS) ; REG <- Var-i
     LOD   B , 108(DS) ; REG <- Var-MAXTEMPO
     SUB   A , B
     BNG   A , ROT2    ; REG < 0 (True)
     LDI   A , #0
     JMP   ROT3
ROT2: ; Rot Teste True
     LDI   A , #1
ROT3:
     STO   A , 118(DS)
     LOD   A , 118(DS)
     BZR   A , ROT4
************************** Expressao ******
     STIF  #0.1000 , 118(DS)
************************** Expressao ******
     STIF  #0.1000 , 120(DS)
************************** Expressao ******
     STI   #0 , 122(DS)
************************** Expressao ******
************************** Expressao ******
     STIF  #0.5000 , 123(DS)
     LODF  A , 123(DS)
     LODF  B , 109(DS)
     MULF  A , B
     STOF  A , 125(DS)
     LODF  A , 125(DS)
     LOD   B , 117(DS)
     CNV   B , B
     MULF  A , B
     STOF  A , 127(DS)
     LODF  A , 127(DS)
     LOD   B , 117(DS)
     CNV   B , B
     MULF  A , B
     STOF  A , 129(DS)
     LOD   A , 115(DS)
     LODF  B , 129(DS)
     CNV   A , A
     SUBF  A , B
     STOF  A , 131(DS)
************************** Expressao ******
     LODF  A , 118(DS)
     LODF  B , 120(DS)
     LOD   C , 122(DS)
     CNV   C , C
     LOD   D , 114(DS)
     CNV   D , D
     LODF  E , 131(DS)
     LOD   F , 116(DS)
     CNV   F , F
     LDI   A , #97
     RTR 
************************** Atribuicao ******
************************** Expressao ******
     STI   #1 , 118(DS)
     LOD   A , 117(DS)
     LOD   B , 118(DS)
     ADD   A , B
     STO   A , 119(DS)
     LOD   A , 119(DS)
     STO   A , 117(DS)
************************** Expressao ******
     STIF  #0.0500 , 118(DS)
     LODF  A , 118(DS)
     TME   A                 ; Pausa <- REG
     JMP   ROT1
ROT4:
     HLT 
