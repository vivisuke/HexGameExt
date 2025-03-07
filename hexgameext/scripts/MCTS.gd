class_name MCTS
extends Node

class MCTSNode:
	#var board_state		# 盤面状態
	var parent = null	# 親ノード (MCTSNode)
	var children = []	# 子ノード (MCTSNode の配列)
	var visits = 0		# 訪問回数
	var wins = 0		# 勝利回数 (ロールアウトで勝利した場合にカウント)
	var streak = 0		# 連勝（ s < 0 for 連敗数？）
	var prior_probability = 0.0		# 事前確率 (必要に応じて使用)
	var move : Vector3i				# 親ノードからこのノードへの着手（x, y, col）
	
	func _init(parent_node, move_made : Vector3i):
		##self.board_state = board_state.copy_from(board_state)
		#self.board_state = Board.new()
		#self.board_state.copy_from(board_state)
		parent = parent_node
		move = move_made
		#self.board_state.put_col(move.x, move.y, col)
		pass
	func to_text(lvl):
		var txt = " ".repeat(lvl*2)
		#txt += "(%d, %d):%d/%d"%[move.x, move.y, wins, visits]
		txt += "%c%d:%d/%d"%[move.x+0x61, move.y+1, wins, visits]
		if visits != 0:
			txt += " = %.1f%%"%(wins*100.0/visits)
		#print(txt)
		return txt
	func print(lvl):
		if lvl != 0:
			#print_self(lvl)
			print(to_text(lvl))
		for node in children:
			if lvl < 3:
				node.print(lvl+1)
		pass
	func select_child_ucb(c_puct: float, parent_visits: int) -> MCTSNode:
		var best_child = null
		var best_ucb = -INF # 無限小
		for child in children:
			var ucb_value = calculate_ucb(child, c_puct, parent_visits)
			#print("ucb_value = ", ucb_value)
			if ucb_value > best_ucb:
				best_ucb = ucb_value
				best_child = child
		return best_child
	func calculate_ucb(node: MCTSNode, c_puct: float, parent_visits: int) -> float:
		if node.visits == 0:
			return INF # 未訪問ノードは優先的に探索
		var exploitation_term : float = float(node.wins) / node.visits # 活用 (勝率)
		var exploration_term : float = c_puct * sqrt(log(parent_visits) / float(node.visits)) # 探索
		return exploitation_term + exploration_term

var root_node: MCTSNode
var board : Board
var board_size: int
var player_color: int	# MCTSで探索するプレイヤーの色
var win_rate = 0.0		# 期待勝率
var policy = null		# 最善着手
var policy_text = ""	# 読み筋テキスト
#var opponent_color: int
#var heuristic_calculator: HexHeuristicValueCalculator # ヒューリスティック計算クラス

#var c_puct = 1.5 # UCB の探索パラメータ (調整可能)
var c_puct = 2.0 # UCB の探索パラメータ (調整可能)
var rollout_depth_limit = 50 # ロールアウトの深さ制限 (無限ループ対策)

func _init(board: Board, p_color: int):		# p_color: 次の手番色
	self.board = board
	board_size = Board.N_HORZ
	player_color = p_color
	#opponent_color = o_color
	root_node = MCTSNode.new(null, Vector3i(-1, -1, -1)) # 初期盤面を根ノードとする
	#heuristic_calculator = HexHeuristicValueCalculator.new(board_size)
	#
	for y in range(board_size):
		for x in range(board_size):
			if board.get_col(x, y) == Board.EMPTY:
				root_node.children.push_back(MCTSNode.new(root_node, Vector3i(x, y, p_color)))
				#root_node.visits += 1
				#root_node.children.back().visits = 1
				#var bd = Board.new()
				#bd.copy_from(board)
				#if bd.playout_random(p_color, x, y) == p_color:
				#	root_node.children.back().wins = 1
func do_rollout(board : Board, node : MCTSNode):
	var bd = Board.new()
	bd.copy_from(board)
	var x = node.move.x
	var y = node.move.y
	var col = node.move.z
	#node.visits += 1
	var wcol = bd.playout_random(col, x, y)
	#if wcol == col:
	#	node.wins += 1
	#if x == 0 && y == 4:
	#	print(node.move, "wcol = ", wcol)
	return wcol
func print_top_children():
	#print("MCTSNode:")
	#root_node.print(2)
	var txt = ""
	var y = 0
	for node in root_node.children:
		if node.move.y != y:
			y = node.move.y
			print(txt)
			txt = ""
		txt += node.to_text(0) + "\t"
	print(txt)
func policy_to_text(node):
	#var txt = "●" if node.move.z == Board.BLUE else "◯"
	var txt = "青" if node.move.z == Board.BLUE else "赤"
	#var txt = "(%d, %d) " % [node.move.x, node.move.y]
	txt += "%c%d(%d/%d) " % [node.move.x+0x61, node.move.y+1, node.wins, node.visits]
	#txt += "%c%d(%d/%d %d) " % [node.move.x+0x61, node.move.y+1, node.wins, node.visits, node.streak]
	if !node.children.is_empty():
		var best = null
		var mxv = -INF
		for chn in node.children:
			if chn.visits != 0 && float(chn.wins)/chn.visits > mxv:
				mxv = float(chn.wins)/chn.visits
				best = chn
		if best != null:
			txt += policy_to_text(best)
	return txt
func print_policy():
	print()
	policy_text = ""
	if true:
		var lst = []
		for node in root_node.children:
			lst.push_back([node.wins*100.0/node.visits, node])
		lst.sort_custom(func(a, b): return a[0] > b[0])
		for i in range(min(5, root_node.children.size())):
			var txt = policy_to_text(lst[i][1])
			policy_text += "%.1f%% %s"%[lst[i][0], txt]
			policy_text += "\n"
			print("%.1f%% "%lst[i][0], txt)
	else:
		var pcnt = policy.wins*100.0/policy.visits
		var txt = policy_to_text(policy)
		print("Best: %.1f%% "%pcnt, txt)
		for node in root_node.children:
			if node.move != policy.move:
				var pcnt2 = node.wins*100.0/node.visits
				if pcnt - pcnt2 <= 5:
					var txt2 = policy_to_text(node)
					print("Other: %.1f%% "%pcnt2, txt2)

func print():
	print("MCTSNode:")
	root_node.print(0)
func add_children():
	pass
func do_expand_node(node, rbd):
	var px = node.move.x
	var py = node.move.y
	var oc = (Board.BLUE + Board.RED) - node.move.z
	for y in range(board_size):
		for x in range(board_size):
			if rbd.get_col(x, y) == Board.EMPTY:
				#node.visits += 1
				node.children.push_back(MCTSNode.new(node, Vector3i(x, y, oc)))
				#node.children.back().visits = 1
				#var bd = Board.new()
				#bd.copy_from(rbd)
				#if bd.playout_random(col, x, y) == col:
				#	root_node.children.back().wins = 1
	
func do_backpropagate(node: MCTSNode, col, wcol):
	#var current_node = node
	while node != null:
		node.visits += 1
		if col == wcol:
			node.wins += 1
		col = (Board.BLUE + Board.RED) - col
		node = node.parent
func do_search(iterations: int) -> Vector2:
	for i in range(iterations):
		var bd = Board.new()
		bd.copy_from(board)
		var col = player_color			# 次の手番色
		#root_node.visits += 1
		var node : MCTSNode = root_node # 探索開始ノードを根ノードに設定
		#node = node.select_child_ucb(c_puct, node.visits) # UCB で子ノードを選択
		var go = false
		var wcol
		while !node.children.is_empty():
			node = node.select_child_ucb(c_puct, node.visits) # UCB で子ノードを選択
			#if node.visits == 0: break		# 未評価ノードの場合
			if bd.put_col(node.move.x, node.move.y, node.move.z):		# 終局
				#node.wins += 1
				wcol = node.move.z
				go = true
				break;
			#col = (Board.BLUE + Board.RED) - col
		if !go:
			# ノード展開（Expansion）
			if node.visits != 0 && node.children.is_empty():
				#var o_col = (Board.BLUE + Board.RED) - col
				do_expand_node(node, bd)
				node = node.children[node.children.size()/2]
			# ロールアウト（Rollout）
			wcol = do_rollout(bd, node)
		#
		do_backpropagate(node, node.move.z, wcol)
	# 出来上がったツリーから最善手を求める
	policy = null
	var mv = Vector2(-1, -1)
	win_rate = -INF
	for node in root_node.children:
		if float(node.wins)/node.visits > win_rate:
			win_rate = float(node.wins)/node.visits
			policy = node
	mv.x = policy.move.x
	mv.y = policy.move.y
	print("max w/v = %.1f%%"%(win_rate*100))
	print("root_node.win/visits = %d/%d" % [root_node.wins, root_node.visits])
	return mv
func search(iterations: int) -> Vector2i:
	for i in range(iterations):
		var node = root_node # 探索開始ノードを根ノードに設定
		#var current_board = root_node.board_state.duplicate() # 盤面コピー

		## 1. 選択 (Selection)
		#while !node.children.is_empty() and !get_possible_moves(current_board).is_empty(): # 葉ノードに到達、または着手可能なマスがなくなるまで
		#	node = node.select_child_ucb(c_puct, node.visits) # UCB で子ノードを選択
		#	current_board.set_cell_color(node.move, get_current_player_color(current_board)) # 選択した着手を盤面に反映


		## 2. 展開 (Expansion)
		#if node.children.is_empty() and !get_possible_moves(current_board).is_empty() and node.visits > 0: # 未展開の葉ノードの場合 (初回訪問時以外)
		#	expand_node(node, current_board)


		## 3. ロールアウト (Rollout)
		#var rollout_result = rollout(current_board.duplicate(), get_current_player_color(current_board)) # ロールアウト実行 (盤面をコピーして渡す)


		## 4. バックプロパゲーション (Backpropagation)
		#backpropagate(node, rollout_result)


	# 探索終了後、最善手を選択 (訪問回数が最も多い子ノード)
	var best_child = select_best_move_child(root_node)
	if best_child != null:
		return best_child.move
	else: # 念のため、最善手が見つからない場合はランダムな空きマスを返す (基本的にはありえないはず)
		var empty_cells = root_node.board_state.get_empty_cells()
		if !empty_cells.is_empty():
			return empty_cells.pick_random()
		else:
			return Vector2i(-1,-1) # 盤面が埋まっている場合など


# --- 探索の各ステップ ---

func expand_node(node: MCTSNode, board: Board):
	var possible_moves = get_possible_moves(board)
	for move in possible_moves:
		var next_board = board.duplicate()
		next_board.set_cell_color(move, get_current_player_color(board)) # 着手
		var child_node = MCTSNode.new(node, move)
		node.children.append(child_node)


func rollout(board: Board, current_player_color: int) -> int: # 1:player_color勝利, -1:opponent_color勝利, 0:引き分け
	var rollout_board = board.duplicate()
	var rollout_player_color = current_player_color
	var rollout_depth = 0

	while rollout_depth < rollout_depth_limit: # 深さ制限
		##if GameLogic.is_game_over(rollout_board):
		##	return GameLogic.get_winner(rollout_board, player_color) # 勝敗判定
		var empty_cells = get_possible_moves(rollout_board)
		if empty_cells.is_empty():
			return 0 # 引き分け (Hexではほぼない)

		# --- ロールアウト着手選択 ---
		# ランダムロールアウト (改善点: ヒューリスティック導入など)
		var move = empty_cells.pick_random()
		rollout_board.set_cell_color(move, rollout_player_color)
		# ---

		rollout_player_color = 3 - rollout_player_color # プレイヤー交代
		rollout_depth += 1

	return 0 # 深さ制限に達した場合 (引き分け扱い or 評価関数で評価するなど、要検討)


func backpropagate(node: MCTSNode, result: int):
	var current_node = node
	while current_node != null:
		current_node.visits += 1
		if get_current_player_color(current_node.board_state) == player_color: # ノードのプレイヤーがMCTSの探索プレイヤーと一致する場合
			current_node.wins += (1 if result == 1 else 0) # ロールアウト結果が探索プレイヤーの勝利ならwinsを増やす
		else: # ノードのプレイヤーが相手プレイヤーの場合
			current_node.wins += (1 if result == -1 else 0) # ロールアウト結果が相手プレイヤーの勝利ならwinsを増やす (評価値を反転させる)

		current_node = current_node.parent


# --- 最善手の選択 ---
func select_best_move_child(node: MCTSNode) -> MCTSNode:
	var best_child = null
	var max_visits = -1

	for child in node.children:
		if child.visits > max_visits:
			max_visits = child.visits
			best_child = child
	return best_child


# --- 補助関数 ---
func get_possible_moves(board: Board) -> Array[Vector2i]:
	return board.get_empty_cells()


func get_current_player_color(board: Board) -> int:
	# 現在の手番のプレイヤーの色を判定 (例: 石の配置数から判定)
	var player1_stones = 0
	var player2_stones = 0
	for cell in board.cells:
		if board.cells[cell] == 1:
			player1_stones += 1
		elif board.cells[cell] == 2:
			player2_stones += 1
	return (2 if player1_stones > player2_stones else 1) # 石が


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
