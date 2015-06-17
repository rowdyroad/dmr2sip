<?php

namespace app\controllers;

use Yii;
use yii\filters\AccessControl;
use yii\web\Controller;

class CommutatorController extends Controller
{
    const PID_FILE = '/var/www/commutator.pid';
    private function getPid()
    {
	if ($pid = file_exists(self::PID_FILE) ? (int)file_get_contents(self::PID_FILE) : 0) {
	    system("ps -p $pid > /dev/null", $ret);
	    if (!$ret) {
		return $pid;
	    }
	}

	return false;
    }

    public function actionState()
    {
	$pid = $this->getPid();
	return [
		'pid'=>$pid,
		'active'=>$pid !== false
	];
    }


    public function actionReload()
    {
	if ($pid = $this->getPid()) {
	    posix_kill($pid, 9);
    	}
	system("/usr/local/commutator/run /var/www");
    }
}
