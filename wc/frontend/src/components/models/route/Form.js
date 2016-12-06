import React, {Component} from 'react'
import * as UI from '../../UIKit'
import {Creator, Grid } from '../../entry'

import {FormSection} from 'redux-form'


const Point = (props) => (
	<UI.Row style={{cursor:'pointer', background: props.input.value == props.point_id ? 'rgba(95, 190, 170, 0.30)' : ''}} onClick={()=> { props.input.onChange(props.point_id)}}>
		<UI.Col1 style={{paddingTop:12, width:32, paddingLeft: 16}}>
			 <div className={"point-item-status point-item-status-" + (props.status ? props.status : 0)} style={{width:16, height:16}}></div>
		</UI.Col1>
		<UI.Col11>
			<b>{props.name}</b>
			<div>
				<small>{props.type}</small>
			</div>
		</UI.Col11>
	</UI.Row>
)

const Points = (props) => {
	return 	<div style={{height:140, overflowY:'scroll', border: '1px solid #E3E3E3', borderRadius: 4, paddingLeft:4}}>
				<Grid {...props} {...props.data} cols={1} plain={true} component={Point}/>
			</div>
}

const SourceSIPConfiguration = (props) => (
	<FormSection name="source_number">
		<UI.RFInput label="From" name="from"/>
		<UI.RFInput label="To" name="to"/>
	</FormSection>
)

const SourceDMRConfiguration = (props) => (
	<FormSection name="source_number">
		<div>
			<label>Call Type</label>
			<UI.Row>
				<UI.Col6>
					<UI.RFRadio label="Private" name="type" value="private"/>
				</UI.Col6>
				<UI.Col6>
					<UI.RFRadio label="Group" name="type" value="group"/>
				</UI.Col6>
			</UI.Row>
		</div>
		<UI.RFInput label="Group ID" name="group_id"/>
		<UI.RFInput label="Radio ID" name="id"/>
	</FormSection>
)


const DestinationDMRConfiguration = (props) => (
	<FormSection name="destination_number">
		<label>Call Type</label>
		<UI.Row>
			<UI.Col4>
				<UI.RFRadio label="Private" name="type" value="private"/>
			</UI.Col4>
			<UI.Col4>
				<UI.RFRadio label="Group" name="type" value="group"/>
			</UI.Col4>
			<UI.Col4>
				<UI.RFRadio label="Channel" name="type" value="channel"/>
			</UI.Col4>
		</UI.Row>
		{props.type === 'private' ? <UI.RFInput label="Radio ID" name="id"/> : null}
		{props.type === 'group'? <UI.RFInput label="Group ID" name="group_id"/> : null}
		{props.type === 'channel' ? <UI.RFInput label="Channel" name="channel"/> : null}
	</FormSection>
)
const DestinationSIPConfiguration = (props) => (
	<FormSection name="destination_number">
		<UI.RFInput label="From" name="from"/>
		<UI.RFInput label="To" name="to"/>
	</FormSection>
)

class Form extends Component {
	componentWillMount = () => {
		this.props.points.actions.fetch();
	}


	render = () => {
		console.log(this.props.data);
		let source = null;
		if (this.props.data && this.props.data.source_point_id && this.props.points.data) {
			let source_point = null;
			if (source_point = this.props.points.data.data.find((item)=> { return item.point_id == this.props.data.source_point_id; })) {
				switch (source_point.type) {
					case 'sip':
						source = <SourceSIPConfiguration {...this.props.data.source_number}/>
					break;

					case 'dmr':
						source = <SourceDMRConfiguration {...this.props.data.source_number}/>
					break;

				}
			}
		}
		let destination = null;
		if (this.props.data && this.props.data.destination_point_id && this.props.points.data) {
			let destination_point = null;
			if (destination_point = this.props.points.data.data.find((item)=> { return item.point_id == this.props.data.destination_point_id; })) {
				switch (destination_point.type) {
					case 'sip':
						destination = <DestinationSIPConfiguration {...this.props.data.destination_number}/>
					break;

					case 'dmr':
						destination = <DestinationDMRConfiguration {...this.props.data.destination_number}/>
					break;

				}
			}
		}

	    return (<UI.Box>
	      <form onSubmit={this.props.onSubmit}>
	          <UI.RFInput label="Name" name="name"/>
	          <UI.Row>
	          	<UI.Col4>
	          		<UI.RFInput label="Source Point" name="source_point_id" component={Points} data={this.props.points}/>
	          		{source ? source : null	}
	          	</UI.Col4>
	          	<UI.Col1 style={{textAlign:'center',fontSize:40, paddingTop:120}}><i className="route-direction fa fa-arrow-right"></i></UI.Col1>
	          	<UI.Col4>
	          		<UI.RFInput label="Destination Point" name="destination_point_id" component={Points} data={this.props.points}/>
	          		{destination ? destination : null	}
	          	</UI.Col4>
	          </UI.Row>

      		<UI.RFCheckbox label="Phone mode" name="phone_mode"/>
      		<UI.RFInput label="Priority" name="order"/>

	          <UI.Button type="submit" disabled={this.props.pristine || this.props.submitting} label="Submit"/>
	          &nbsp;
	          <UI.Button type="button" color="white" onClick={this.props.onClose} label="Close"/>
	      </form>
	    </UI.Box>)
    }
}

export default Form;