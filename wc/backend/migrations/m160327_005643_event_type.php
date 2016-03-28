<?php

use yii\db\Migration;

class m160327_005643_event_type extends Migration
{
    public function up()
    {
        $this->addColumn('events','type','string');
        $this->addColumn('events','end_time','datetime default NULL');
        \app\models\Event::updateAll(['type'=>'call']);
    }

    public function down()
    {
        $this->dropColumn('events','type');
    }
}
