import React, { Component } from 'react'
import {Link} from 'react-router';
import TimeAgo from 'react-timeago'
import * as UI from '../../UIKit'

class Events extends Component
{
	componentWillMount = () => {
		this.props.actions.fetch(1);
	}

	more = () =>
	{
		this.props.actions.fetch(this.props.data._meta.currentPage + 1, true);
	}

	render = () => {

		if (!this.props.data) {
			return null; //!todo: add loader and error handling
		}

		if (!this.props.data.length) {
			return <UI.NoContent/>
		}

		return (
					<div>
						<div id="cd-timeline" className="cd-container">
							{this.props.data.data.map((event) => {
								return React.createElement(this.props.component, {...event, key: event.event_id});
							})}
						</div>
						{this.props.data._meta.currentPage < this.props.data._meta.pageCount ?
							<div style={{textAlign:'center'}}>
								<UI.Button onClick={this.more} label="More"/>
							</div>
						: null
						}
					</div>
		)
	}
}

export default Events;