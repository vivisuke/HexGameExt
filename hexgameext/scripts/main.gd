extends Node2D

func _ready() -> void:
	var bd = CBoard.new()
	bd.set_width(11)
	print("board width = ", bd.get_width())
	bd.put_color(1, 1, 2)
	print("(1, 1) = ", bd.get_color(1, 1))
	pass
