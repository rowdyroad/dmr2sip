import React, { Component, PropTypes } from 'react'
import { connect } from 'react-redux'
import Request from '../utils/Request';

class PageLoader extends Component
{
    state = {
        items: [],
        page: 1,
        has_more:true,
        loading:true
    }

    fetchEvents = (page) => {
        this.setState({loading: true});
        Request.get(this.props.url + '?page=' + page).then((data) => {
            this.setState({
                                loading:false,
                                items: [...this.state.items, ...data.data],
                                page: data._meta.currentPage,
                                has_more: data._meta.currentPage < data._meta.pageCount
                            });
        });
    }

    componentWillMount = () => {
        this.fetchEvents(1);
    }

    getMore = () => {
        this.fetchEvents(this.state.page + 1);
    }

    render = () => {

        let child_props = {}
        child_props[this.props.dataAttribute ? this.props.dataAttribute : 'items'] = this.state.items;

        return (
                <div className={this.props.className}>
                    {React.cloneElement(this.props.children, child_props)}
                    <div className="row" style={{textAlign:'center'}}>
                        <div className="col-md-12">
                        {this.state.has_more ?
                            <button className="page-loader-more btn btn-primary" onClick={this.getMore} disabled={this.state.loading}>More</button> : null }
                        </div>
                    </div>
                </div>
            )
    }
}

export default PageLoader;