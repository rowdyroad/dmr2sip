import React from 'react'
import * as UI from '../../UIKit'
import Point from '../point/Preview'


export default (props) => {
	if (!props.data) {
        return null;
    }

    let deleted = (<div style={{height:80}}> <i className="ti-close" style={{color: 'red',}}></i><br/>deleted</div>)
    return (
		<UI.Box>
	        <UI.Area>
	            <UI.Row>
	                <UI.Col10>
	                    <h1>{props.data.name}</h1>
	                </UI.Col10>
	                <UI.Col2 style={{textAlign:'right'}}>
	                    <UI.Button to={'/routes/' + props.data.route_id + '/update'} icon="ti-pencil"/>
	                    &nbsp;
	                    <UI.Button to={'/routes/' + props.data.route_id + '/delete'} color="danger" icon="ti-trash"/>
	                </UI.Col2>
	            </UI.Row>
	            <UI.Row>
	            	<UI.Col2>
	            		{props.data.source_point ? <Point {...props.data.source_point}/> : deleted}
	            	</UI.Col2>
					<UI.Col1><i className="route-direction fa fa-arrow-right"></i></UI.Col1>
	            	<UI.Col2>
	            		{props.data.destination_point ? <Point {...props.data.destination_point}/> : deleted}
	            	</UI.Col2>
	            </UI.Row>

	            <UI.Row>
	            	<UI.Col6>
	            		<UI.Property label="Phone mode">{props.data.phone_mode ? 'On' : 'Off'}</UI.Property>
	            		<UI.Property label="Priority">{props.data.order}</UI.Property>
	            	</UI.Col6>
	            </UI.Row>
	        </UI.Area>
	    </UI.Box>
	  )
}