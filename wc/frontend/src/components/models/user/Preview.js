import React from 'react'
import * as UI from '../../UIKit'

export default (props) => (
	<div>
		<UI.Row>
			<UI.Col4>
				#{props.user_id}
			</UI.Col4>
			<UI.Col8 style={{textAlign:'right'}}>
				<UI.Button to={'/users/' + props.user_id + '/update'} icon="ti-pencil"/>
			</UI.Col8>
		</UI.Row>

        <h1 className="point-item-name">{props.name}</h1>
        <h4 className="point-item-type">{props.username}</h4>
        {props.is_admin ? <div>Full Access</div>: <div>Restricted Access</div>}

    </div>
)