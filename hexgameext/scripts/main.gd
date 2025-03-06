extends Node2D

func _ready() -> void:
	var bd = CBoard.new(11)
	print("board width = ", bd.get_width())
	pass
