import React, { Component, PropTypes } from 'react'
import * as UI from '../../components/UIKit'

const RemoveModal = (props) => (
	<UI.Modal isOpen={this.props.open} title="Removing current item" onClose={this.props.actions.hide}>
	    <div style={{padding:'1em'}}>Do you really want to remove current element <b>{this.props.data.name}</b>?</div>
	    <UI.Button label="Remove" color="danger" onClick={()=>{ this.props.actions.delete(this.props.data)}}/>
	    &nbsp;
	    <UI.Button label="Cancel" color="white" onClick={this.props.actions.hide}/>
	</UI.Modal>
)

RemoveModal.propType = {
	open: React.PropTypes.bool.isRequired,
	actions: React.PropTypes.shape({
				hide: React.PropTypes.func.isRequired,
			}),
	data: React.PropTypes.object.isRequired,
	data: React.PropTypes.shape({
				name: React.PropTypes.name.isRequired
			})
}

export default RemoveModal;