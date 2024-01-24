; ModuleID = 'MyModule'
source_filename = "MyModule"

define i32 @main() {
entry:
  br i1 false, label %ifblock, label %endblock

ifblock:                                          ; preds = %entry
  %0 = call i32 @exit(i32 2)
  br label %endblock

endblock:                                         ; preds = %ifblock, %entry
  %1 = call i32 @exit(i32 9)
  ret i32 0
}

declare i32 @exit(i32)
