; ModuleID = 'MyModule'
source_filename = "MyModule"

define i32 @main() {
entry:
  br label %scope1

scope1:                                           ; preds = %entry
  br label %scope2

scope2:                                           ; preds = %scope1
  br label %scope3

scope3:                                           ; preds = %scope2
  %0 = call i32 @exit(i32 10)
  br label %continuation3

continuation3:                                    ; preds = %scope3
  br label %continuation2

continuation2:                                    ; preds = %continuation3
  br label %continuation1

continuation1:                                    ; preds = %continuation2
  ret i32 0
}

declare i32 @exit(i32)
