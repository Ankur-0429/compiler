; ModuleID = 'MyModule'
source_filename = "MyModule"

define i32 @main() {
entry:
  br label %scope1

scope1:                                           ; preds = %entry
  %0 = call i32 @exit(i32 5)
  br label %continuation1

continuation1:                                    ; preds = %scope1
  ret i32 0
}

declare i32 @exit(i32)
