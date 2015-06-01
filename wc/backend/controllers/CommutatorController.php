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
	return file_exists(self::PID_FILE) ? (int)file_get_contents(self::PID_FILE) : 0;
    }

    public function actionState()
    {
	$active = false;
	if ($pid = $this->getPid()) {
	    system("ps -p $pid > /dev/null", $ret);
	    $active = !(bool)$ret; 
	}
	return [
		'pid'=>$pid,
		'active'=>$active
	];
    }


    public function actionReload()
    {
	if ($pid = $this->getPid()) {
	    posix_kill($pid, 1);
    	} else {
	    system("/usr/local/dmr2sip/build/commutator >> /var/www/commutator.log & echo $! > /var/www/commutator.pid");
	}
    }
}
