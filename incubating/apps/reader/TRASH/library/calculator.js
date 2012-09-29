// calculator.js
// 9/28/2012 jichi

var curVal = 0
var memory = 0
var lastOp = ""
var previousOperator = ""
var digits = ""

function disabled(op) {
  if (op == "." && digits.toString().search(/\./) != -1) {
    return true
  } else if (op == window.squareRoot &&  digits.toString().search(/-/) != -1) {
    return true
  } else {
    return false
  }
}

function digitPressed(op)
{
  if (disabled(op))
    return
  if (digits.toString().length >= 14)
    return
  if (lastOp.toString().length == 1 && ((lastOp >= "0" && lastOp <= "9") || lastOp == ".") ) {
    digits = digits + op.toString()
    display.appendDigit(op.toString())
  } else {
    digits = op
    display.appendDigit(op.toString())
  }
  lastOp = op
}

function operatorPressed(op)
{
  if (disabled(op))
    return
  lastOp = op

  if (previousOperator == "+") {
    digits = Number(digits.valueOf()) + Number(curVal.valueOf())
  } else if (previousOperator == "−") {
    digits = Number(curVal) - Number(digits.valueOf())
  } else if (previousOperator == "×") {
    digits = Number(curVal) * Number(digits.valueOf())
  } else if (previousOperator == "÷") {
    digits = Number(Number(curVal) / Number(digits.valueOf())).toString()
  } else if (previousOperator == "=") {
  }

  if (op == "+" || op == "−" || op == "×" || op == "÷") {
    previousOperator = op
    curVal = digits.valueOf()
    display.displayOperator(previousOperator)
    return
  }

  if (op == "=") {
    display.newLine("=", digits.toString())
  }

  curVal = 0
  previousOperator = ""

  if (op == "1/x") {
    digits = (1 / digits.valueOf()).toString()
  } else if (op == "x^2") {
    digits = (digits.valueOf() * digits.valueOf()).toString()
  } else if (op == "Abs") {
    digits = (Math.abs(digits.valueOf())).toString()
  } else if (op == "Int") {
    digits = (Math.floor(digits.valueOf())).toString()
  } else if (op == window.plusminus) {
    digits = (digits.valueOf() * -1).toString()
  } else if (op == window.squareRoot) {
    digits = (Math.sqrt(digits.valueOf())).toString()
  } else if (op == "mc") {
    memory = 0;
  } else if (op == "m+") {
    memory += digits.valueOf()
  } else if (op == "mr") {
    digits = memory.toString()
  } else if (op == "m-") {
    memory = digits.valueOf()
  } else if (op == window.leftArrow) {
    digits = digits.toString().slice(0, -1)
    if (digits.length == 0) {
      digits = "0"
    }
  } else if (op == "Off") {
    Qt.quit();
  } else if (op == "C") {
    digits = "0"
  } else if (op == "AC") {
    curVal = 0
    memory = 0
    lastOp = ""
    digits ="0"
  }
}

// EOF
