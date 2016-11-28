import React, { Component, PropTypes } from 'react';
import {Link} from 'react-router';
import '../assets'
import CenterLayout from '../components/CenterLayout';

class NotFound extends Component
{
	render = () => {
		return (<CenterLayout>
            		<div className="ex-page-content text-center">
                		<div className="text-error"><span className="text-primary">4</span><span className="text-pink">0</span><span className="text-info">4</span></div>
                		<h2>Bad Request</h2>
                		<br/>
                		<p className="text-muted">Your browser sent an invalid request</p>
                		<br/>
                		<Link className="btn btn-default waves-effect waves-light" to="/"> Return Home</Link>
            		</div>
	            </CenterLayout>
	        	);
	}
}

export default NotFound;