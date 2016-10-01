<?php
namespace app\components;
use Yii;

class Docker extends \yii\base\Component
{
	public $socket;

	private function exec_($url, $params = false)
	{
		$ch = curl_init();
        curl_setopt($ch, CURLOPT_HTTPHEADER, ['Content-Type:application/json']);
        curl_setopt($ch, CURLOPT_UNIX_SOCKET_PATH, $this->socket);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
		curl_setopt($ch, CURLOPT_POST, $params !== false );
        if (is_array($params)) {
        	curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($params));
        }
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_HEADER, false);

        if (($ret = curl_exec($ch)) !== false) {
        	return !empty($ret) ? $ret : true;
        }
        curl_close($ch);
        return false;
	}

	public function log($container, $out = 1, $tail = 100)
	{
		return $this->exec_(sprintf("http:/containers/%s/logs?stdout=%d&tail=%d", $container, $out, $tail));
	}

	public function restart($container)
	{
		return $this->exec_(sprintf("http:/containers/%s/restart", $container), true);
	}

	public function status($container)
	{
		return json_decode($this->exec_(sprintf("http:/containers/%s/json", $container)));
	}

	public function exec($container, $command)
	{
		$chunks = explode(" ", $command);
		$command = [];
		for ($i = 0; $i < count($chunks); ++$i) {
			$ch = [];
			if (in_array($chunks[$i][0], ['\'','"'])) {
				for ($j = $i; $j < count($chunks); ++$j) {
					$ch[] = trim($chunks[$j], '\'"');
					if ($chunks[$j][strlen($chunks[$j]) - 1] == $chunks[$i][0]) {
						$i = $j;
						break;
					}
				}
			} else {
				$ch[] = $chunks[$i];
			}
			$command[] = implode(" ", $ch);
		}

		$context = [
						"AttachStdin" => true,
						"AttachStdout" => true,
						"AttachStderr" => true,
						"Cmd" => $command
					];

		if ($data = json_decode($this->exec_(sprintf("http:/containers/%s/exec", $container), $context))) {
			return $this->exec_(sprintf("http:/exec/%s/start", $data->Id), ["Detach" =>false, "Tty" => false ]);
		}
		return false;
	}
}