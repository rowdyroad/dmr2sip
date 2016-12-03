import React, { Component } from 'react'
import { connect } from 'react-redux'
import {Link} from 'react-router';
import TimeAgo from 'react-timeago'
import * as Actions from '../actions'
import * as UI from '../components/UIKit'
class Event extends Component
{
	state = {
		expanded: false
	}
	toggleExpanded = () => {
		this.setState({expanded: !this.state.expanded});
	}

	render = () => {
		return (
					<div className="event cd-timeline-block">
				        <div className="cd-timeline-img cd-success">
				            <i className="fa fa-phone"></i>
				        </div>
        				<div className="cd-timeline-content">
			            	<h3>
			            		{this.props.source_point
			            			? <span>
			            				<Link to={"/points/" + this.props.source_point.point_id}>{this.props.source_point.name}</Link>
			            				(<small>{this.props.source_point.type}</small>)
			            			</span>
			            			: 'DELETED'
			            		}
			            		<i className="route-direction fa fa-arrow-right"></i>
			            		{
			            			this.props.route 	? 	<span>
			            										{this.props.route.destination_point ? <Link to="/points/{this.props.route.destination_point.point_id}">{this.props.route.destination_point.name}</Link> : 'DELETED'}
			            										{this.props.route.destination_point ? (<small>{this.props.route.destination_point.type}</small>) : null}
			            									</span>
			            						  		: 	<i style={{color:'red'}} className="ti-close"></i>
			            		}
			            	</h3>
			            	{ this.props.route ? <div className="event-param">Route: <Link to={"/routes/" + this.props.route.route_id}>{this.props.route.name}</Link></div> : null }
			            	{ this.props.route ? <div className="event-param">Duration: {this.props.duration} sec</div> : null }
			            	<i className="ti-more" style={{cursor:'pointer'}} onClick={this.toggleExpanded}></i>
						   	<div className="event-extend" style={{display: this.state.expanded ? 'block' : 'none'}}>
						        <hr style={{margin: 4}}/>
						        { this.props.source 	? 	<span>
						        							{Object.keys(this.props.source).map((value) => {
						        								return (this.props.source[value].length > 0 ?
						        												<span className="event-extend-item" key={value}>
						        													<b>{value}</b>: {this.props.source[value]}
						        												</span>
						        												: null
						        												)
						        							})}
						        							</span>
						        						: 	null
						        }
						        {
						        	this.props.destination ? <span>
						        								<i className="route-direction fa fa-arrow-right"></i>
						        							{Object.keys(this.props.destination).map((value) => {
						        								return (this.props.destination[value].length > 0 ?
						        												<span className="event-extend-item" key={value}>
						        													<b>{value}</b>: {this.props.destination[value]}
						        												</span>
						        												: null
						        												)
						        							})}
						        							</span>
						        						: 	null
						        }
						    </div>
						    <span className="cd-date"><TimeAgo date={this.props.time}/></span>
						</div>
					</div>
				)
	}

}

class Events extends Component
{
	componentWillMount = () => {
		this.props.actions.fetch(1);
	}

	more = () =>
	{
		this.props.actions.fetch(this.props.meta.currentPage + 1, true);
	}

	render = () => {

		if (!this.props.data) {
			return null; //!todo: add loader and error handling
		}
		return (
					<div>
						<div id="cd-timeline" className="cd-container">
							{this.props.data.map((event) => (
								<Event key={event.event_id} {...event}></Event>
							))}
						</div>
						{this.props.fetching ? <div>Loading</div> : null}
						{this.props.meta.currentPage < this.props.meta.pageCount ?
							<div style={{textAlign:'center'}}>
								<UI.Button onClick={this.more} label="More"/>
							</div>
						: null
						}
					</div>
		)
	}
}

let mapState = (state) => {
	return {
			fetching: state.main.getIn(['events', 'fetching']),
			data: state.main.hasIn(['events','response','data']) ? state.main.getIn(['events','response','data']).toJS() : null,
			meta: state.main.hasIn(['events','response','_meta']) ? state.main.getIn(['events','response','_meta']).toJS() : null,
	}
}

let mapDispatch = (dispatch) => {
  return {
    actions: {
      fetch: (page, append) => {
        dispatch(Actions.Fetch("events", {
        									url: "/api/events",
        									params: {page: page}
        								},
        								{
        									concat: append ? 'data' : false
        								}



        								));
      }
    }
  }
}

export default connect(mapState, mapDispatch)(Events)




