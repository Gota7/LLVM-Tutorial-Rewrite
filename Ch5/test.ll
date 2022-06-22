; ModuleID = 'test.bc'
source_filename = "Super Epic JIT"

define double @fact(double %x) {
entry:
  %cmptmp = fcmp ult double %x, 2.000000e+00
  br i1 %cmptmp, label %ifcont, label %else

else:                                             ; preds = %entry
  %subtmp = fadd double %x, -1.000000e+00
  %calltmp = call double @fact(double %subtmp)
  %multmp = fmul double %x, %calltmp
  br label %ifcont

ifcont:                                           ; preds = %else, %entry
  %iftmp = phi double [ %multmp, %else ], [ 1.000000e+00, %entry ]
  ret double %iftmp
}

define double @__anon_expr() {
entry:
  br label %loop

loop:                                             ; preds = %loop, %entry
  %i = phi double [ 0.000000e+00, %entry ], [ %nextvar, %loop ]
  %calltmp = call double @fact(double %i)
  %nextvar = fadd double %i, 1.000000e+00
  %cmptmp = fcmp ult double %i, 7.000000e+00
  br i1 %cmptmp, label %loop, label %afterloop

afterloop:                                        ; preds = %loop
  ret double 0.000000e+00
}
