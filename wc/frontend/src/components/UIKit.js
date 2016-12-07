import React from 'react'
import {Link} from 'react-router'
import {default as ReactModal} from 'react-modal'
import { Field } from 'redux-form/immutable'

export const RFInput = (props) => (
	<div className="form-group">
		{props.label ? <label className="control-label">{props.label}</label> : null}
		<Field
				className="form-control"
				component="input"
				{...props}
				/>
	</div>
)

export const RFRadio = (props) => (
	<div className="form-inline">
		<div className="form-group">
			<Field
					className="form-control"
					component="input"
					type="radio"
					{...props}
					style={{...props.style ||{}, marginTop:-3}}
					/>
			<label style={{fontWeight:'normal',paddingLeft:4}}>{props.label}</label>
		</div>
	</div>
)

export const RFCheckbox = (props) => (
	<div className="form-inline">
		<div className="form-group">
			<Field
					className="form-control"
					component="input"
					type="checkbox"
					{...props}
					style={{...props.style ||{}, marginTop:-3}}
					/>
			<label style={{fontWeight:'normal',paddingLeft:4}}>{props.label}</label>
		</div>
	</div>
)


export const RFTextarea = (props) => (
	<div className="form-group">
		{props.label ? <label className="control-label">{props.label}</label> : null}
		<Field
				className="form-control"
				component="textarea"
				{...props}
				/>
	</div>
)


export const RFSelect = (props) => (
	<div className="form-group">
		{props.label ? <label className="control-label">{props.label}</label> : null}
		<Field
				className="form-control"
				component="select"
				{...props}
				>
		{props.children}
		</Field>
	</div>
)



export const Modal = (props) =>(
	<ReactModal
          isOpen={props.isOpen}
          shouldCloseOnOverlayClick={false}
          style={{overlay: { zIndex:10000}, content:{left:'25%', top:'25%', right:'25%', bottom:'auto'}}}
        >
        <Row>
        	<Col11>
        		<h1>{props.title}</h1>
        	</Col11>
        	<Col1>
        		{props.onClose ? <Button color="white" icon="glyphicon glyphicon-remove" onClick={props.onClose}/> : null}
        	</Col1>
        </Row>

          {props.children}
    </ReactModal>
)
export const Input = (props) => (
	<div className="form-group">
		{props.label ? <label className="control-label">{props.label}</label> : null}
		<input
				className="form-control"
				disabled={props.disabled ? true : false}
				type={props.type ? props.type : 'text'}
				name={props.name}
				value={props.value}
				placeholder={props.placeholder}
				onChange={props.onChange}/>
	</div>
)

export const Select = (props) => (
	<div className="form-group">
		{ props.label ? <label className="control-label">{props.label}</label> : null}
		<select
				className="form-control"
				disabled={props.disabled ? true : false}
				value={props.value}
				name={props.name}
				placeholder={props.placeholder}
				onChange={props.onChange}>
			{props.children}
		</select>
	</div>
)

export const Box = (props) => (
	<div {...props} className={"card-box " + props.className || ""}>
		{props.children}
	</div>
)

export const Property = (props) => (
	<div style={{marginBottom:'1em'}}>
		<label>{props.label}</label>
		<div>{props.children}</div>
	</div>
)

export const Button = (props) => {

	let classes = ["btn", "waves-effect", "waves-light", "btn-" + (props.color ? props.color : "default")];

	if (props.icon) {
		classes.push("btn-icon");
	}

	let button = (
					<button
							type={props.type ? props.type : 'button'}
							className={classes.join(' ')}
							style={props.style}
							disabled={props.disabled}
							onClick={props.onClick}>
							{props.icon ? <i className={props.icon}></i> : null}
							{props.icon && props.label ? <span>&nbsp;</span> : null}
							{props.label ? <span>{props.label}</span> : null}
					</button>
				)
	return props.to ? (<Link to={props.to}>{button}</Link>) : button;
}


export const Area = (props) => (
	<div>{props.children}</div>
)

export const Row = (props) => (
	<div {...props} className={"row " + (props.className || "")}>
		{props.children}
	</div>
)

export const ProgressBar = (props) => (
	<div className="progress">
	  <div className={"progress-bar progress-bar-" + (props.type || "info")}
	  		role="progressbar"
	  		aria-valuenow={props.progress}
	  		aria-valuemin={props.min || 0}
	  		aria-valuemax={props.max || 100}
	  		style={{width: props.progress +'%'}}>
	    {props.progress}%
	  </div>
	</div>
)

export const NoContent = (props) => (
	<div className="no-content">There is no any data here</div>
)

export const LoadingContent = (props) => (
	<div className="loading-content">Loading...</div>
)


export const Grid = (props) => {

	if (!props.data) {
		return props.loadingComponent ? React.createElement(props.loadingComponent, props) : <LoadingContent {...props}/>
	} else if (!props.data.length) {
		return props.noContentComponent ? React.createElement(props.noContentComponent, props) : <NoContent {...props}/>
	}

	return  (<Row>
				{props.data.map((item,i) => {
					if (!item) return item;
					return <Col
									width={12 / (props.cols || 4)}
									key={i}
									style={{...(props.style || {}), padding:'0.5em'}}>
										{props.plain ? React.createElement(props.component, {...props, ...item}) :
												<Box>{React.createElement(props.component, {...props, ...item})}</Box>}
							</Col>;
				})}
			</Row>)
}


export const Col = (props) => (
	<div className={"col-md-" + props.width} {...props}>
		{React.Children.map(props.children, (item) => {
			if (!React.isValidElement(item)) return item;
			return React.cloneElement(item, item.props);
		})}
	</div>
)
export const Col1 = (props) => ( <Col {...props} width={1}>{props.children}</Col>)
export const Col2 = (props) => ( <Col {...props} width={2}>{props.children}</Col>)
export const Col3 = (props) => ( <Col {...props} width={3}>{props.children}</Col>)
export const Col4 = (props) => ( <Col {...props} width={4}>{props.children}</Col>)
export const Col5 = (props) => ( <Col {...props} width={5}>{props.children}</Col>)
export const Col6 = (props) => ( <Col {...props} width={6}>{props.children}</Col>)
export const Col7 = (props) => ( <Col {...props} width={7}>{props.children}</Col>)
export const Col8 = (props) => ( <Col {...props} width={8}>{props.children}</Col>)
export const Col9 = (props) => ( <Col {...props} width={9}>{props.children}</Col>)
export const Col10 = (props) => ( <Col {...props} width={10}>{props.children}</Col>)
export const Col11 = (props) => ( <Col {...props} width={11}>{props.children}</Col>)
export const Col12 = (props) => ( <Col {...props} width={12}>{props.children}</Col>)



